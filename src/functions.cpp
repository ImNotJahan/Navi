#include "lisp.h"
#include "functions.h"

Error function_head(Atom args, Atom* result)
{
	check_args(args, 1);

	if (nullp(head(args))) *result = null;
	else if (!(head(args).type == Atom::PAIR || head(args).type == Atom::STRING)) 
		return Error{ Error::TYPE, "Expected string or pair", "HEAD"};
	else *result = head(head(args));

	return NOERR;
}

Error function_tail(Atom args, Atom* result)
{
	check_args(args, 1);

	if (nullp(head(args))) *result = null;
	else if (!(head(args).type == Atom::PAIR || head(args).type == Atom::STRING))
		return Error{ Error::TYPE, "Expected string or pair", "TAIL" };
	else *result = tail(head(args));

	[](float x, float y)
	{
		return x + y;
	};

	return NOERR;
}

Error function_cons(Atom args, Atom* result)
{
	check_args(args, 2);

	*result = cons(head(args), head(tail(args)));

	return NOERR;
}

// Base function for the add, subtract, divide, and multiply functions. Handles all error checking
Error arithmetic(const std::function<float(float, float)>& f, Atom args, Atom *result)
{
	Atom a, b;

	check_args(args, 2);

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
	check_args(args, 1);

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

		case Atom::MACRO:
			*result = sym("MACRO");
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
	check_args(args, 2);

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
	check_args(args, 2);

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
	check_args(args, 2);

	Atom a, b;
	a = head(args);
	b = head(tail(args));

	if (!both_type(a, b, BOOLEAN)) return Error{ Error::TYPE, "Booleans expected", "AND"};

	*result = (a.value.boolean && b.value.boolean) tf;

	return NOERR;
}

Error function_or(Atom args, Atom* result)
{
	check_args(args, 2);

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

	check_args(args, 2);

	fn = head(args);
	args = head(tail(args));

	if (!listp(args)) return Error{ Error::SYNTAX, "Arguments are not proper list", "APPLY"};

	return apply(fn, args, result);
}

Error function_print(Atom args, Atom* result)
{
	Atom expr;

	check_args(args, 1);
	expr = head(args);

	print_expr(expr);
	print_expr(make_character('\n'));

	return NOERR;
}

Error function_load(Atom args, Atom* result)
{
	// Need to add strings before able to implement this func
	return NOERR;
}