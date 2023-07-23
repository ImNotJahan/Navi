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

Atom multiply_bignums(Atom a, Atom b)
{
	Atom result = int_to_bignum(0);

	// Loop through digits in b
	int depth = 0;
	while (!nullp(b))
	{
		Atom high, low;
		split_bignum(copy_list(b), 1, &low, &high);

		Atom copy = copy_list(a);
		int multicand = head(tail(high)).value.integer;
		int index = depth;
		while (!nullp(copy))
		{
			Atom h, l;

			split_bignum(copy_list(copy), 1, &l, &h);
			Atom prod = int_to_bignum(head(tail(h)).value.integer * multicand);
			prod = shift_bignum(prod, index);

			result = add_bignums(result, prod);

			copy = l;
			index++;
		}

		b = low;
		depth++;
	}

	return result;
	/*
	say_expr(a); say_expr(make_character(' '));

	int lengths[2] = { bignum_length(copy_list(a)), bignum_length(copy_list(b)) };

	if (lengths[0] + lengths[1] < 7)
	{
		say_expr(null);
		return int_to_bignum(head(tail(a)).value.integer * head(tail(b)).value.integer);
	}
	
	int middle = std::max(lengths[0], lengths[1]);
	middle /= 2;

	middle = std::min(std::min((lengths[0] - 1), (lengths[1] - 1)), middle);

	Atom low1, low2, high1, high2;

	split_bignum(copy_list(a), middle, &low1, &high1);
	split_bignum(copy_list(b), middle, &low2, &high2);

	say_expr(low1); say_expr(make_character(' '));
	say_expr(high1); say_expr(make_character(' '));

	Atom z0 = multiply_bignums(low1, low2);
	Atom z1 = multiply_bignums(add_bignums(low1, high1), add_bignums(low2, high2));
	Atom z2 = shift_bignum(multiply_bignums(high1, high2), middle * 2);

	return add_bignums(z0, add_bignums(z2, shift_bignum(subtract_bignums(subtract_bignums(z1, z2), z0), middle)));*/
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

Atom shift_bignum(Atom bignum, int times)
{
	for (int i = 0; i < times; i++)
	{
		bignum = shift_bignum(bignum);
	}

	return bignum;
}

Atom shift_bignum(Atom bignum)
{
	Atom decimal = head(bignum);
	Atom result = null;
	bignum = tail(bignum);
	
	list_reverse(&bignum);

	int carry = 0;
	while (!nullp(bignum))
	{
		int num = head(bignum).value.integer;

		if (int_length(num) < 9)
		{
			result = cons(make_int(num * 10 + carry), result);
			carry = 0;
		}
		else
		{
			result = cons(make_int(num % 100000000 * 10 + carry), result);
			carry = num / 100000000;
		}

		bignum = tail(bignum);
	}

	if (carry != 0) result = cons(make_int(carry), result);

	result = cons(decimal, result);
	result.type = Atom::BIGNUM;

	return result;
}

void split_bignum(Atom bignum, int middle, Atom* low, Atom* high)
{
	int skip = bignum_length(bignum) - middle;

	bignum = tail(bignum);
	list_reverse(&bignum);

	*low = null;
	*high = null;

	int carry = 0;
	int offset = 0;
	
	for (int i = 0; i < middle;)
	{
		int num = head(bignum).value.integer;
		bool last = nullp(tail(bignum));
		
		if (!last && 9 < middle || last && int_length(num) < middle)
		{
			*high = cons(make_int(num), *high);
			if (last) i += int_length(num);
			else i += 9;
		}
		else
		{
			offset = (int)pow(10, middle);
			*high = cons(make_int(num % offset), *high);
			carry = num / offset;

			if (last) offset = int_length(carry);
			else offset = 8;

			i += middle;
		}

		bignum = tail(bignum);
	}
	
	int a = (int)pow(10, 9 - offset);
	int b = (int)pow(10, offset);

	*high = cons(make_int(-1), *high);
	high->type = Atom::BIGNUM;

	while (!nullp(bignum))
	{
		int v = head(bignum).value.integer;
		int num = carry; 

		num += (v % a) * b;
		carry = (v / a);

		*low = cons(make_int(num), *low);

		bignum = tail(bignum);
	}

	if (carry != 0) *low = cons(make_int(carry), *low);

	if (nullp((*low))) return;

	*low = cons(make_int(-1), *low);
	low->type = Atom::BIGNUM;
}