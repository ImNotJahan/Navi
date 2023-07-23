#include "../include/main.h"
#include "../include/bignum_math.h"
#include "../include/data.h"
#include "../include/numbers.h"

Atom add_bignums(Atom a, Atom b)
{
	int decimal_pos[2] = { head(a).value.integer, head(b).value.integer };

	// If both are negative, cancel out signs as makes it easier
	if (head(tail(a)).value.integer < 0 && head(tail(b)).value.integer < 0)
	{
		head(tail(a)).value.integer = std::abs(head(tail(a)).value.integer);
		head(tail(b)).value.integer = std::abs(head(tail(b)).value.integer);

		Atom result = add_bignums(a, b);
		head(tail(result)).value.integer *= -1;

		return result;
		
	} // If xor a b are negative then just pass to subtract func
	else if (head(tail(a)).value.integer < 0)
	{
		head(tail(a)).value.integer = std::abs(head(tail(a)).value.integer);
		return subtract_bignums(b, a);
	}
	else if (head(tail(b)).value.integer < 0)
	{
		head(tail(b)).value.integer = std::abs(head(tail(b)).value.integer);
		return subtract_bignums(a, b);
	}

	a = tail(a);
	b = tail(b);

	// Reverse lists as want to add the smallest numbers first
	list_reverse(&a);
	list_reverse(&b);

	Atom result = null;

	int carry = 0;
	while (!nullp(a) && !nullp(b))
	{
		int temp = head(a).value.integer + head(b).value.integer + carry;

		if (int_length(temp) > 9)
		{
			carry = temp / 1000000000 % 10; // Gets front digit
			temp = remove_first_digit(temp);
		}
		else
		{
			carry = 0;
		}
		
		result = cons(make_int(temp), result);
		
		a = tail(a);
		b = tail(b);
	}
	
	while (!nullp(a))
	{
		Atom num = head(a);
		if (carry != 0)
		{
			num.value.integer += carry;
			
			if (int_length(num.value.integer) > 9)
			{
				carry = num.value.integer / 1000000000 % 10; // Gets front digit
				num.value.integer = remove_first_digit(num.value.integer);
			}
			else
			{
				carry = 0;
			}
		}

		result = cons(num, result);
		a = tail(a);
	}
	
	while (!nullp(b))
	{
		Atom num = head(b);
		if (carry != 0)
		{
			num.value.integer += carry;

			if (int_length(num.value.integer) > 9)
			{
				carry = num.value.integer / 1000000000 % 10; // Gets front digit
				num.value.integer = remove_first_digit(num.value.integer);
			}
			else
			{
				carry = 0;
			}
		}

		result = cons(num, result);
		b = tail(b);
	}

	if (carry != 0)
	{
		result = cons(make_int(carry), result);
	}
	
	result = cons(make_int(-1), result);

	result.type = Atom::BIGNUM;
	return result;
}

Atom subtract_bignums(Atom a, Atom b)
{
	int decimal_pos[2] = { head(a).value.integer, head(b).value.integer };

	// If both are negative, cancel out signs as makes it easier
	if (head(tail(a)).value.integer < 0 && head(tail(b)).value.integer < 0)
	{
		head(tail(a)).value.integer = std::abs(head(tail(a)).value.integer);
		head(tail(b)).value.integer = std::abs(head(tail(b)).value.integer);
	} // If b is negative then just pass to add func
	else if (head(tail(a)).value.integer < 0)
	{
		head(tail(a)).value.integer = std::abs(head(tail(a)).value.integer);
		
		// -a - b = -(a + b)
		Atom result = add_bignums(b, a);
		head(tail(result)).value.integer *= -1;

		return result;
	}
	else if (head(tail(b)).value.integer < 0)
	{
		head(tail(b)).value.integer = std::abs(head(tail(b)).value.integer);
		return add_bignums(a, b);
	}
	
	// a - b = -(b - a) & easier to determine a - b when a > b
	if (bignum_less(a, b))
	{
		Atom result = subtract_bignums(b, a);
		head(tail(result)).value.integer *= -1;

		return result;
	}

	a = tail(a);
	b = tail(b);
	
	// Reverse lists as want to add the smallest numbers first
	list_reverse(&a);
	list_reverse(&b);

	Atom result = null;

	int borrow = 0;
	while (!nullp(a) && !nullp(b))
	{
		int temp = head(a).value.integer - head(b).value.integer + borrow;

		if (temp < 0)
		{
			temp += 1000000000;
			borrow = -1;
		}
		else
		{
			borrow = 0;
		}

		result = cons(make_int(temp), result);

		a = tail(a);
		b = tail(b);
	}

	while (!nullp(a))
	{
		Atom num = head(a);
		if (borrow != 0)
		{
			num.value.integer += borrow;

			if (num.value.integer < 0)
			{
				num.value.integer += 1000000000;
				borrow = -1;
			}
			else
			{
				borrow = 0;
			}
		}

		result = cons(num, result);
		a = tail(a);
	}

	while (!nullp(b))
	{
		Atom num = head(b);
		if (borrow != 0)
		{
			num.value.integer += borrow;

			if (num.value.integer < 0)
			{
				num.value.integer += 1000000000;
				borrow = -1;
			}
			else
			{
				borrow = 0;
			}
		}

		result = cons(num, result);
		b = tail(b);
	}

	// Remove leading zeros
	while (!nullp(tail(result)))
	{
		if (head(result).value.integer != 0) break;

		result = tail(result);
	}

	result = cons(make_int(-1), result);

	result.type = Atom::BIGNUM;
	return result;
}

// Also very slow
Atom multiply_bignums(Atom a, Atom b)
{
	Atom result = int_to_bignum(0);

	while (head(tail(b)).value.integer != 0)
	{
		result = add_bignums(result, copy_list(a));
		b = subtract_bignums(b, int_to_bignum(1));
	}

	return result;
}


// Very slow, but works for now
Atom divide_bignums(Atom a, Atom b)
{
	Atom quotient = int_to_bignum(0);

	// !(a < b) = a >= b
	while (!(bignum_less(a, b)))
	{
		a = subtract_bignums(copy_list(a), copy_list(b));
		quotient = add_bignums(quotient, int_to_bignum(1));
	}

	return quotient;
}

bool bignum_less(Atom a, Atom b)
{
	bool negative[2] = { (head(tail(a)).value.integer < 0), (head(tail(b)).value.integer < 0) };
	bool bothNegative;
	
	int a_val = head(tail(a)).value.integer;
	int b_val = head(tail(b)).value.integer;

	// If one is negative and the other isn't, it definitely doesn't have a higher value
	if (negative[0] && !negative[1]) return true;
	if (!negative[0] && negative[1]) return false;
	// If both are negative length means the opposite
	bothNegative = negative[0] && negative[1];

	// Length is a good indicator of size
	if (bignum_length(a) < bignum_length(b)) return !bothNegative;
	if (bignum_length(a) > bignum_length(b)) return bothNegative;

	a = tail(tail(a));
	b = tail(tail(b));

	// If values are equal, search deeper till inequality 
	while (a_val == b_val)
	{
		if (nullp(a)) return false;

		a_val = head(a).value.integer;
		b_val = head(b).value.integer;

		a = tail(a);
		b = tail(b);
	}

	return a_val < b_val;
}