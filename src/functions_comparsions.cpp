#include "../include/main.h"
#include "../include/functions.h"
#include "../include/bignum_math.h"

Error function_eq(Atom args, Atom* result)
{
	check_args(args, 2, "=");

	Atom a, b;

	a = head(args);
	b = head(tail(args));

	// Numbers should be comparable
	if (is_number(a) && is_number(b))
	{
		if (both_type(a, b, INTEGER)) *result = type_eq(a, b, integer);
		else if (both_type(a, b, FLOAT)) *result = type_eq(a, b, float_);
		else if (both_type(a, b, BIGNUM)) *result = compare_lists(a, b) tf;
		else if (a.type == Atom::INTEGER && b.type == Atom::FLOAT) *result = a.value.integer == b.value.float_ tf;
		else if (a.type == Atom::FLOAT && b.type == Atom::INTEGER) *result = a.value.float_ == b.value.integer ? true_ : false_;
		else if (a.type == Atom::BIGNUM && b.type == Atom::INTEGER) *result = compare_lists(a, int_to_bignum(b.value.integer)) tf;
		else if (a.type == Atom::INTEGER && b.type == Atom::BIGNUM) *result = compare_lists(int_to_bignum(a.value.integer), b) tf;
		else *result = false_;

		return NOERR;
	}
	// Allows comparing type with null
	else if (a.type == Atom::NULL_ || b.type == Atom::NULL_)
	{
		*result = a.type == b.type tf;

		return NOERR;
	}
	else if (a.type != b.type)
	{
		*result = false_;
		return NOERR;
	}

	if (both_type(a, b, SYMBOL)) *result = type_eq(a, b, symbol);
	else if (both_type(a, b, CHARACTER)) *result = type_eq(a, b, character);
	else if (both_type(a, b, BOOLEAN)) *result = type_eq(a, b, boolean);
	else if (both_type(a, b, PAIR) || both_type(a, b, STRING)) *result = compare_lists(a, b) tf;
	else if (both_type(a, b, RATIO))
	{
		*result = a.value.ratio.denominator == b.value.ratio.denominator &&
			a.value.ratio.numerator == b.value.ratio.numerator tf;
	}
	else return Error{ Error::TYPE, "Cannot compare type", "=" };

	return NOERR;
}

Error function_less(Atom args, Atom* result)
{
	check_args(args, 2, "<");

	Atom a, b;
	a = head(args);
	b = head(tail(args));

	if (!(is_number(a) && is_number(b)))
		return Error{ Error::TYPE, "Number expected", "LESS" };

	if (both_type(a, b, INTEGER)) *result = a.value.integer < b.value.integer tf;
	else if (both_type(a, b, FLOAT)) *result = a.value.float_ < b.value.float_ tf;
	else if (a.type == Atom::INTEGER && b.type == Atom::FLOAT) *result = a.value.integer < b.value.float_ tf;
	else if (a.type == Atom::FLOAT && b.type == Atom::INTEGER) *result = a.value.float_ < b.value.integer tf;
	else // for bignums
	{
		if (both_type(a, b, BIGNUM)) *result = bignum_less(a, b) tf;
		else if (a.type == Atom::BIGNUM)
		{
			if (b.type == Atom::INTEGER) *result = bignum_less(a, int_to_bignum(b.value.integer)) tf;
			else *result = bignum_less(a, int_to_bignum(b.value.float_)) tf;
		}
		else if (b.type == Atom::BIGNUM)
		{
			if (a.type == Atom::INTEGER)*result = bignum_less(int_to_bignum(a.value.integer), b) tf;
			else *result = bignum_less(int_to_bignum(a.value.float_), b) tf;
		}
	}

	return NOERR;
}