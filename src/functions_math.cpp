#include "../include/main.h"
#include "../include/functions.h"
#include "../include/bignum_math.h"

// God this is ugly

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
			// To support
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
			// To support
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
			// To support
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
			// To support
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
			int a_val = a.value.integer;
			int b_val = b.value.integer;

			// Make sure multiplication won't go over nine digits
			if (b_val != 0 && a_val > 999999999 / b_val ||
				b_val != 0 && a_val < -999999999 / b_val)
			{
				*result = multiply_bignums(int_to_bignum(a.value.integer), int_to_bignum(b.value.integer));
			}
			else
			{
				*result = make_int(a_val * b_val);
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
			*result = multiply_bignums(a, b);
		}
		else if (b.type == Atom::INTEGER)
		{
			*result = multiply_bignums(a, int_to_bignum(b.value.integer));
		}
		else if (b.type == Atom::FLOAT)
		{
			// To support
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
			// To support
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
			*result = divide_bignums(int_to_bignum(a.value.integer), b);
		}
	}
	else if (a.type == Atom::BIGNUM)
	{
		if (b.type == Atom::BIGNUM)
		{
			*result = divide_bignums(a, b);
		}
		else if (b.type == Atom::INTEGER)
		{
			*result = divide_bignums(a, int_to_bignum(b.value.integer));
		}
		else if (b.type == Atom::FLOAT)
		{
			// To support
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
			// To support
		}
	}

	return NOERR;
}