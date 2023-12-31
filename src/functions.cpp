#include "../include/main.h"
#include "../include/functions.h"
#include "../include/numbers.h"

#include <iostream>

Error function_head(Atom args, Atom* result)
{
	check_args(args, 1, "HEAD");

	if (nullp(head(args))) *result = null;
	else if (!(head(args).type == Atom::PAIR || head(args).type == Atom::STRING || head(args).type == Atom::BIGNUM || head(args).type == Atom::RATIO))
		return Error{ Error::TYPE, "Expected string or pair", "HEAD"};
	else *result = head(head(args));

	return NOERR;
}

Error function_tail(Atom args, Atom* result)
{
	check_args(args, 1, "TAIL");

	if (nullp(head(args))) *result = null;
	else if (!(head(args).type == Atom::PAIR || head(args).type == Atom::STRING || head(args).type == Atom::BIGNUM || head(args).type == Atom::RATIO))
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

		case Atom::EXPAND:
			*result = sym("EXPAND");
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

	return NOERR;
}

Error function_float(Atom args, Atom* result)
{
	check_args(args, 1, "FLOAT");

	Atom value = head(args);
	Atom float_{ Atom::FLOAT };

	if (value.type == value.FLOAT) float_.value.float_ = value.value.float_;
	else if (value.type == value.INTEGER) float_.value.float_ = value.value.integer;
	else if (value.type == value.RATIO)
	{
		if (head(value).type == Atom::INTEGER && tail(value).type == Atom::INTEGER)
		{
			float_.value.float_ = (float)head(value).value.integer / tail(value).value.integer;
		}
	}
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
	else if (value.type == value.BIGNUM) integer.value.integer = head(value).value.integer;
	else if (value.type == value.RATIO)
	{
		if (head(value).type != Atom::INTEGER || tail(value).type != Atom::INTEGER)
		{
			return Error{ Error::SYNTAX, "Ratio can only contain integers", "INT" };
		}
		integer.value.integer = head(value).value.integer / tail(value).value.integer;
	}
	else return Error{ Error::TYPE, "Expected number", "INT"};

	*result = integer;

	return NOERR;
}

Error function_ratio(Atom args, Atom* result)
{
	check_args(args, 2, "RATIO");

	Atom numerator = head(args);
	Atom denominator = head(tail(args));

	if (!(numerator.type == Atom::INTEGER || numerator.type == Atom::BIGNUM &&
		denominator.type == Atom::INTEGER || denominator.type == Atom::BIGNUM)) return Error{ Error::TYPE, "Expected integers", "RATIO" };

	*result = make_ratio(numerator, denominator);

	return NOERR;
}

Error function_string(Atom args, Atom* result)
{
	check_args(args, 1, "STRING");

	Atom value = head(args);
	Atom string{ Atom::STRING };
	Error err = NOERR;

	std::string str = "";

	switch(value.type)
	{
		case Atom::PAIR:
			if (!listp(value)) return Error{ Error::SYNTAX, "Improper list", "STRING" };
			if (!list_of_type_p(value, Atom{ Atom::CHARACTER })) return Error{ Error::SYNTAX, "List must only contain characters", "STRING" };

			string.value.pair = value.value.pair;
			*result = string;
			break;

		case Atom::SYMBOL:
			err = make_string(*value.value.symbol, result);
			break;

		case Atom::STRING:
			*result = value;
			break;

		case Atom::INTEGER:
			err = make_string(std::to_string(value.value.integer), result);
			break;

		case Atom::FLOAT:
			err = make_string(std::to_string(value.value.float_), result);
			break;

		case Atom::NULL_:
			err = make_string("null", result);
			break;
	
		case Atom::CHARACTER:
			err = make_string(std::string(1, value.value.character), result);
			break;

		case Atom::BOOLEAN:
			if(value.value.boolean) err = make_string("true", result);
			else err = make_string("false", result);
			break;

		case Atom::BIGNUM:
			str += std::to_string(head(value).value.integer);
			value = tail(value);

			while (!nullp(value))
			{
				int nums = head(value).value.integer;

				for (int i = int_length(nums) + 1; i < 10; i++) str += "0";

				str += std::to_string(nums);
				value = tail(value);
			}

			err = make_string(str, result);
			break;

		default:
			return Error{ Error::TYPE, "Type cannot be turned into string", "STRING" };
	}

	return err;
}

Error function_bignum(Atom args, Atom* result)
{
	check_args(args, 1, "BIGNUM");
	
	Atom value = head(args);
	Atom bignum{ Atom::BIGNUM };

	if (value.type == Atom::PAIR)
	{
		if (!listp(value)) return Error{ Error::SYNTAX, "Improper list", "BIGNUM" };
		if (list_length(value) < 1) return Error{ Error::SYNTAX, "List must contain at least one integer", "BIGNUM" };
		
		if (!list_of_type_p(value, Atom{ Atom::INTEGER })) return Error{ Error::SYNTAX, "List must only contain integers", "BIGNUM" };

		// Loop through pair to make sure no ints after head are negative (eg 1234 -4832)

		Atom possible_bignum = tail(copy_list(value)); // Ignore first int (head)
		while (!nullp(possible_bignum))
		{
			if (head(possible_bignum).value.integer < 0) return Error{ Error::SYNTAX, "Bignum integer parts should not be negative after head", "BIGNUM" };
			possible_bignum = tail(possible_bignum);
		}

		bignum.value.pair = value.value.pair;
		*result = bignum;
	}
	else if (value.type == Atom::STRING)
	{
		if (is_int(*value.value.symbol) || is_float(*value.value.symbol))
		{
			*result = make_bignum(*value.value.symbol);
		}
		else
		{
			return Error{ Error::SYNTAX, "String must be of number", "BIGNUM" };
		}
	}
	else if (value.type == Atom::INTEGER)
	{
		bignum = cons(value, null);
		bignum.type = Atom::BIGNUM;

		*result = bignum;
	}
	else if (value.type == Atom::BIGNUM)
	{
		*result = value;
	}
	else
	{
		return Error{ Error::TYPE, "Type cannot be turned into bignum", "BIGNUM" };
	}

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

Error function_listen(Atom args, Atom* result)
{
	check_args(args, -1, "LISTEN");

	std::string input;
	std::getline(std::cin, input);

	Error err;

	err = make_string(input, result);

	if (err.type) return err;
	return NOERR;
}