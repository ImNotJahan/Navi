#include "../include/main.h"
#include "../include/functions.h"
#include "../include/bignum_math.h"

Error function_add(Atom args, Atom* result)
{
	Atom a, b;

	check_args(args, 2, "+");

	a = head(args);
	b = head(tail(args));

	if (!(is_number(a) && is_number(b))) return Error{ Error::TYPE, "Expected number", "+" };

	if (a.type == Atom::INTEGER)
	{
		if (b.type == Atom::INTEGER)
		{
			int value = a.value.integer + b.value.integer;

			// Numbers over 9 digits are turned to bignums
			if (value > 999999999 || value < -999999999)
			{
				*result = add_bignums(int_to_bignum(a.value.integer), int_to_bignum(b.value.integer));
			}
			else
			{
				*result = make_int(value);
			}
		}
		else if(b.type == Atom::FLOAT)
		{
			*result = make_float(a.value.integer + b.value.float_);
		}
		else if (b.type == Atom::BIGNUM)
		{
			*result = add_bignums(int_to_bignum(a.value.integer), b);
		}
	}
	else if (a.type == Atom::BIGNUM)
	{
		if (b.type == Atom::BIGNUM)
		{
			*result = add_bignums(a, b);
		}
		else if (b.type == Atom::INTEGER)
		{
			*result = add_bignums(a, int_to_bignum(b.value.integer));
		}
		else if (b.type == Atom::FLOAT)
		{
			return Error{ Error::SYNTAX, "Can't add float and bignum", "+" };
		}
	}
	else if(a.type == Atom::FLOAT)
	{
		if (b.type == Atom::INTEGER)
		{
			*result = make_float(a.value.float_ + b.value.integer);
		}
		else if(b.type == Atom::FLOAT)
		{
			*result = make_float(a.value.float_ + b.value.float_);
		}
		else if (b.type == Atom::BIGNUM)
		{
			return Error{ Error::SYNTAX, "Can't add float and bignum", "+" };
		}
	}

	return NOERR;
}

Error function_subtract(Atom args, Atom* result)
{
	Atom a, b;

	check_args(args, 2, "-");

	a = head(args);
	b = head(tail(args));

	if (!(is_number(a) && is_number(b))) return Error{ Error::TYPE, "Expected number", "-" };

	if (a.type == Atom::INTEGER)
	{
		if (b.type == Atom::INTEGER)
		{
			int value = a.value.integer - b.value.integer;

			// Numbers over 9 digits are turned to bignums
			if (value > 999999999 || value < -999999999)
			{
				*result = subtract_bignums(int_to_bignum(a.value.integer), int_to_bignum(b.value.integer));
			}
			else
			{
				*result = make_int(value);
			}
		}
		else if(b.type == Atom::FLOAT)
		{
			*result = make_float(a.value.integer - b.value.float_);
		}
		else if (b.type == Atom::BIGNUM)
		{
			*result = subtract_bignums(int_to_bignum(a.value.integer), b);
		}
	}
	else if (a.type == Atom::BIGNUM)
	{
		if (b.type == Atom::BIGNUM)
		{
			*result = subtract_bignums(a, b);
		}
		else if (b.type == Atom::INTEGER)
		{
			*result = subtract_bignums(a, int_to_bignum(b.value.integer));
		}
		else if (b.type == Atom::FLOAT)
		{
			return Error{ Error::SYNTAX, "Can't subtract float and bignum", "-" };
		}
	}
	else if(a.type == Atom::FLOAT)
	{
		if (b.type == Atom::INTEGER)
		{
			*result = make_float(a.value.float_ - b.value.integer);
		}
		else if(b.type == Atom::FLOAT)
		{
			*result = make_float(a.value.float_ - b.value.float_);
		}
		else if (b.type == Atom::BIGNUM)
		{
			return Error{ Error::SYNTAX, "Can't subtract float and bignum", "-" };
		}
	}

	return NOERR;
}

Error function_multiply(Atom args, Atom* result)
{
	Atom a, b;

	check_args(args, 2, "*");

	a = head(args);
	b = head(tail(args));

	if (!(is_number(a) && is_number(b))) return Error{ Error::TYPE, "Expected number", "*" };

	if (a.type == Atom::INTEGER)
	{
		if (b.type == Atom::INTEGER)
		{
			int val = a.value.integer * b.value.integer;

			// Make sure multiplication won't go over nine digits
			if (a.value.integer != 0  && val / a.value.integer != b.value.integer)
			{
				*result = karatsuba_bignums(int_to_bignum(a.value.integer), int_to_bignum(b.value.integer));
			}
			else
			{
				*result = make_int(val);
			}
		}
		else if (b.type == Atom::FLOAT)
		{
			*result = make_float(a.value.integer * b.value.float_);
		}
		else if (b.type == Atom::BIGNUM)
		{
			*result = multiply_bignums(int_to_bignum(a.value.integer), b);
		}
	}
	else if (a.type == Atom::BIGNUM)
	{
		if (b.type == Atom::BIGNUM)
		{
			*result = karatsuba_bignums(a, b);
		}
		else if (b.type == Atom::INTEGER)
		{
			*result = karatsuba_bignums(a, int_to_bignum(b.value.integer));
		}
		else if (b.type == Atom::FLOAT)
		{
			return Error{ Error::SYNTAX, "Can't multiply float and bignum", "*" };
		}
	}
	else if (a.type == Atom::FLOAT)
	{
		if (b.type == Atom::INTEGER)
		{
			*result = make_float(a.value.float_ * b.value.integer);
		}
		else if (b.type == Atom::FLOAT)
		{
			*result = make_float(a.value.float_ * b.value.float_);
		}
		else if (b.type == Atom::BIGNUM)
		{
			return Error{ Error::SYNTAX, "Can't multiply float and bignum", "*" };
		}
	}

	return NOERR;
}

Error function_divide(Atom args, Atom* result)
{
	Atom a, b;

	check_args(args, 2, "/");

	a = head(args);
	b = head(tail(args));

	if (!(is_number(a) && is_number(b))) return Error{ Error::TYPE, "Expected number", "/" };

	switch (b.type)
	{
		case Atom::INTEGER:
			if(b.value.integer == 0) return Error{ Error::SYNTAX, "Can't divide a number by zero", "/" };
			break;

		case Atom::FLOAT:
			if (b.value.float_ == 0) return Error{ Error::SYNTAX, "Can't divide a number by zero", "/" };
			break;

		case Atom::BIGNUM:
			if(head(b).value.integer == 0) return Error{ Error::SYNTAX, "Can't divide a number by zero", "/" };
	}

	if (a.type == Atom::INTEGER)
	{
		if (b.type == Atom::INTEGER)
		{
			// Division can't overflow
			*result = make_int(a.value.integer / b.value.integer);
		}
		else if (b.type == Atom::FLOAT)
		{
			*result = make_float(a.value.integer / b.value.float_);
		}
		else if (b.type == Atom::BIGNUM)
		{
			*result = head(divide_bignums(int_to_bignum(a.value.integer), b));
		}
	}
	else if (a.type == Atom::BIGNUM)
	{
		if (b.type == Atom::BIGNUM)
		{
			*result = head(divide_bignums(a, b));
		}
		else if (b.type == Atom::INTEGER)
		{
			*result = head(divide_bignums(a, int_to_bignum(b.value.integer)));
		}
		else if (b.type == Atom::FLOAT)
		{
			return Error{ Error::SYNTAX, "Can't divide float and bignum", "/" };
		}
	}
	else if (a.type == Atom::FLOAT)
	{
		if (b.type == Atom::INTEGER)
		{
			*result = make_float(a.value.float_ / b.value.integer);
		}
		else if (b.type == Atom::FLOAT)
		{
			*result = make_float(a.value.float_ / b.value.float_);
		}
		else if (b.type == Atom::BIGNUM)
		{
			return Error{ Error::SYNTAX, "Can't divide float and bignum", "/" };
		}
	}

	return NOERR;
}

Error function_remainder(Atom args, Atom* result)
{
	Atom a, b;

	check_args(args, 2, "REMAINDER");

	a = head(args);
	b = head(tail(args));

	if (!(a.type == Atom::INTEGER || a.type == Atom::BIGNUM) ||
		!(b.type == Atom::INTEGER || b.type == Atom::BIGNUM)) return Error{ Error::TYPE, "Expected integer or bignum", "REMAINDER" };

	switch (b.type)
	{
		case Atom::INTEGER:
			if (b.value.integer == 0) return Error{ Error::SYNTAX, "Can't divide a number by zero", "REMAINDER" };
			break;

		case Atom::BIGNUM:
			if (head(b).value.integer == 0) return Error{ Error::SYNTAX, "Can't divide a number by zero", "REMAINDER" };
	}

	if (a.type == Atom::INTEGER)
	{
		if (b.type == Atom::INTEGER)
		{
			// Division can't overflow
			*result = make_int(a.value.integer % b.value.integer);
		}
		else if (b.type == Atom::BIGNUM)
		{
			*result = tail(divide_bignums(int_to_bignum(a.value.integer), b));
		}
	}
	else if (a.type == Atom::BIGNUM)
	{
		if (b.type == Atom::BIGNUM)
		{
			*result = tail(divide_bignums(a, b));
		}
		else if (b.type == Atom::INTEGER)
		{
			*result = tail(divide_bignums(a, int_to_bignum(b.value.integer)));
		}
	}

	return NOERR;
}

Error function_shift(Atom args, Atom* result)
{
	check_args(args, 2, "SHIFT");

	if (!both_type(head(args), head(tail(args)), INTEGER))
		return Error{ Error::TYPE, "Expected integers" };

	int number, shift;
	number = head(args).value.integer;
	shift = head(tail(args)).value.integer;

	if(shift > 0) *result = make_int(number << shift);
	else *result = make_int(number >> -shift);

	return NOERR;
}