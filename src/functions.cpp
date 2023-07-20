#include "../include/main.h"
#include "../include/functions.h"

Error function_head(Atom args, Atom* result)
{
	check_args(args, 1, "HEAD");

	if (nullp(head(args))) *result = null;
	else if (!(head(args).type == Atom::PAIR || head(args).type == Atom::STRING || head(args).type == Atom::BIGNUM))
		return Error{ Error::TYPE, "Expected string or pair", "HEAD"};
	else *result = head(head(args));

	return NOERR;
}

Error function_tail(Atom args, Atom* result)
{
	check_args(args, 1, "TAIL");

	if (nullp(head(args))) *result = null;
	else if (!(head(args).type == Atom::PAIR || head(args).type == Atom::STRING || head(args).type == Atom::BIGNUM))
		return Error{ Error::TYPE, "Expected string or pair", "TAIL" };
	else *result = tail(head(args));

	return NOERR;
}

Error function_pair(Atom args, Atom* result)
{
	check_args(args, 2, "PAIR");

	*result = cons(head(args), head(tail(args)));

	return NOERR;
}

// Base function for the add, subtract, divide, and multiply functions. Handles all error checking
Error arithmetic(const std::function<float(float, float)>& f, Atom args, Atom *result)
{
	Atom a, b;

	check_args(args, 2, "ARITHMETIC");

	a = head(args);
	b = head(tail(args));

	if (!(is_number(a) && is_number(b))) return Error{ Error::TYPE, "Expected number", "ARITHMETIC"};

	// For adding ints and floats
	if (a.type == Atom::INTEGER)
	{
		if (b.type == Atom::INTEGER)
		{
			*result = make_int(f(a.value.integer, b.value.integer));
		}
		else
		{
			*result = make_float(f(a.value.integer, b.value.float_));
		}
	}
	else
	{
		if (b.type == Atom::INTEGER)
		{
			*result = make_float(f(a.value.float_, b.value.integer));
		}
		else
		{
			*result = make_float(f(a.value.float_, b.value.float_));
		}
	}

	return NOERR;
}

Error function_add(Atom args, Atom* result)
{
	auto add = [](float x, float y) { return x + y; };

	return arithmetic(add, args, result);
}

Error function_subtract(Atom args, Atom* result)
{
	auto subtract = [](float x, float y) { return x - y; };

	return arithmetic(subtract, args, result);
}

Error function_multiply(Atom args, Atom* result)
{
	auto multiply = [](float x, float y) { return x * y; };

	return arithmetic(multiply, args, result);
}

Error function_divide(Atom args, Atom* result)
{
	auto divide = [](float x, float y) { return x / y; };

	return arithmetic(divide, args, result);
}

Error function_type(Atom args, Atom* result)
{
	check_args(args, 1, "TYPE_OF");

	switch(head(args).type)
	{
		case Atom::CHARACTER:
			*result = sym("CHARACTER");
			break;

		case Atom::INTEGER:
			*result = sym("INTEGER");
			break;

		case Atom::FLOAT:
			*result = sym("FLOAT");
			break;

		case Atom::FUNCTION:
			*result = sym("BUILTIN_FUNCTION");
			break;

		case Atom::SYMBOL:
			*result = sym("SYMBOL");
			break;

		case Atom::PAIR:
			*result = sym("PAIR");
			break;

		case Atom::NULL_:
			*result = null;
			break;

		case Atom::CLOSURE:
			*result = sym("CLOSURE");
			break;

		case Atom::BOOLEAN:
			*result = sym("BOOL");
			break;

		case Atom::EXPANSION:
			*result = sym("EXPANSION");
			break;

		case Atom::STRING:
			*result = sym("STRING");
			break;

		case Atom::RATIO:
			*result = sym("RATIO");
			break;

		case Atom::BIGNUM:
			*result = sym("BIGNUM");
			break;

		default:
			return Error{ Error::TYPE, "Unknown type", "TYPE"};
	}

	return NOERR;
}

Error function_eq(Atom args, Atom* result)
{
	check_args(args, 2, "=");

	Atom a, b;

	a = head(args);
	b = head(tail(args));

	// Floats and ints should be comparable
	if (is_number(a) && is_number(b))
	{
		if (both_type(a, b, INTEGER)) *result = type_eq(a, b, integer);
		else if (both_type(a, b, FLOAT)) *result = type_eq(a, b, float_);
		else if (a.type == Atom::INTEGER) *result = a.value.integer == b.value.float_ tf;
		else *result = a.value.float_ == b.value.integer ? true_ : false_;

		return NOERR;
	}
	// Allows comparing type with null
	else if (a.type == Atom::NULL_ || b.type == Atom::NULL_)
	{
		*result = a.type == b.type tf;

		return NOERR;
	}
	else if (a.type != b.type) 
		return Error{ Error::TYPE, "Arguments must be same type (or numbers)", "EQL"};

	if (both_type(a, b, SYMBOL)) *result = type_eq(a, b, symbol);
	else if (both_type(a, b, CHARACTER)) *result = type_eq(a, b, character);
	else if (both_type(a, b, BOOLEAN)) *result = type_eq(a, b, boolean);
	else return Error{ Error::TYPE, "Cannot compare type", "EQL"};

	return NOERR;
}

Error function_less(Atom args, Atom* result)
{
	check_args(args, 2, "<");

	Atom a, b;
	a = head(args);
	b = head(tail(args));

	if (!(is_number(a) && is_number(b))) 
		return Error{ Error::TYPE, "Number expected", "LESS"};

	if (both_type(a, b, INTEGER)) *result = a.value.integer < b.value.integer tf;
	else if (both_type(a, b, FLOAT)) *result = a.value.float_ < b.value.float_ tf;
	else if (a.type == Atom::INTEGER) *result = a.value.integer < b.value.float_ tf;
	else *result = a.value.float_ < b.value.integer tf;

	return NOERR;
}

Error function_and(Atom args, Atom* result)
{
	check_args(args, 2, "AND");

	Atom a, b;
	a = head(args);
	b = head(tail(args));

	if (!both_type(a, b, BOOLEAN)) return Error{ Error::TYPE, "Booleans expected", "AND"};

	*result = (a.value.boolean && b.value.boolean) tf;

	return NOERR;
}

Error function_or(Atom args, Atom* result)
{
	check_args(args, 2, "OR");

	Atom a, b;
	a = head(args);
	b = head(tail(args));

	if (!both_type(a, b, BOOLEAN)) return Error{ Error::TYPE, "Booleans expected", "OR"};

	*result = (a.value.boolean || b.value.boolean) tf;

	return NOERR;
}

Error function_apply(Atom args, Atom* result)
{
	Atom fn;

	check_args(args, 2, "APPLY");

	fn = head(args);
	args = head(tail(args));

	if (!listp(args)) return Error{ Error::SYNTAX, "Arguments are not proper list", "APPLY"};

	return apply(fn, args, result);
}

Error function_say(Atom args, Atom* result)
{
	Atom expr;

	check_args(args, 1, "SAY");
	expr = head(args);

	say_expr(expr);
	say_expr(make_character('\n'));

	return NOERR;
}

Error function_float(Atom args, Atom* result)
{
	check_args(args, 1, "FLOAT");

	Atom value = head(args);
	Atom float_{ Atom::FLOAT };

	if (value.type == value.FLOAT) float_.value.float_ = value.value.float_;
	else if (value.type == value.INTEGER) float_.value.float_ = value.value.integer;
	else if (value.type == value.RATIO) float_.value.float_ = (float)value.value.ratio.numerator / value.value.ratio.denominator;
	else return Error{ Error::TYPE, "Expected number", "FLOAT"};

	*result = float_;

	return NOERR;
}

Error function_int(Atom args, Atom* result)
{
	check_args(args, 1, "INT");

	Atom value = head(args);
	Atom integer{ Atom::INTEGER };

	if (value.type == value.FLOAT) integer.value.integer = value.value.float_;
	else if (value.type == value.INTEGER) integer.value.integer = value.value.integer;
	else if (value.type == value.RATIO) integer.value.integer = value.value.ratio.numerator / value.value.ratio.denominator;
	else return Error{ Error::TYPE, "Expected number", "INT"};

	*result = integer;

	return NOERR;
}

Error function_ratio(Atom args, Atom* result)
{
	check_args(args, 2, "RATIO");

	Atom numerator = head(args);
	Atom denominator = head(tail(args));
	Atom ratio{ Atom::RATIO };

	if (!both_type(numerator, denominator, INTEGER)) return Error{ Error::TYPE, "Expected integers", "RATIO" };

	ratio.value.ratio.numerator = numerator.value.integer;
	ratio.value.ratio.denominator = denominator.value.integer;

	*result = ratio;

	return NOERR;
}

Error function_numerator(Atom args, Atom* result)
{
	check_args(args, 1, "NUMERATOR");

	Atom ratio = head(args);
	Atom numerator{ Atom::INTEGER };

	if (ratio.type != Atom::RATIO) return Error{ Error::TYPE, "Expected ratio", "NUMERATOR" };

	numerator.value.integer = ratio.value.ratio.numerator;
	*result = numerator;

	return NOERR;
}

Error function_denominator(Atom args, Atom* result)
{
	check_args(args, 1, "DENOMINATOR");

	Atom ratio = head(args);
	Atom denominator{ Atom::INTEGER };

	check_type(ratio, RATIO, "ratio", "DENOMINATOR");

	denominator.value.integer = ratio.value.ratio.denominator;
	*result = denominator;

	return NOERR;
}

Error function_error(Atom args, Atom* result)
{
	const int arg_length = list_length(args);

	if (arg_length < 1 || arg_length > 3) return ARGNUM("ERROR");

	Error err;
	Atom errorType = head(args);

	check_type(errorType, STRING, "string", "ERROR");
	
	std::string errorTypeString = to_string(errorType);
	
	if (errorTypeString == "SYNTAX") err.type = Error::SYNTAX;
	else if (errorTypeString == "OK") err.type = Error::OK;
	else if (errorTypeString == "ARGS") err.type = Error::ARGS;
	else if (errorTypeString == "UNBOUND") err.type = Error::UNBOUND;
	else if (errorTypeString == "TYPE") err.type = Error::TYPE;
	else if (errorTypeString == "EMPTY") err.type = Error::EMPTY;
	else return Error{ Error::SYNTAX, "Error type doesn't exist", "ERROR" };
	
	if (arg_length > 1)
	{
		Atom details = head(tail(args));

		check_type(details, STRING, "string", "ERROR");

		err.details = to_string(details);

		if (arg_length == 3)
		{
			Atom from = head(tail(tail(args)));

			check_type(from, STRING, "string", "ERROR");

			err.from = to_string(from);
		}
	}

	return err;
}