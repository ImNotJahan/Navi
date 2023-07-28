#include "../include/main.h"
#include "../include/bignum_math.h"
#include "../include/data.h"
#include "../include/numbers.h"

Atom add_bignums(Atom a, Atom b)
{
	// If both are negative, cancel out signs as makes it easier
	if (head(a).value.integer < 0 && head(b).value.integer < 0)
	{
		head(a).value.integer = std::abs(head(a).value.integer);
		head(b).value.integer = std::abs(head(b).value.integer);

		Atom result = add_bignums(a, b);
		head(result).value.integer *= -1;

		return result;
		
	} // If xor a b are negative then just pass to subtract func
	else if (head(a).value.integer < 0)
	{
		head(a).value.integer = std::abs(head(a).value.integer);
		return subtract_bignums(b, a);
	}
	else if (head(b).value.integer < 0)
	{
		head(b).value.integer = std::abs(head(b).value.integer);
		return subtract_bignums(a, b);
	}

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
	
	result.type = Atom::BIGNUM;
	return result;
}

Atom subtract_bignums(Atom a, Atom b)
{
	// If both are negative, cancel out signs as makes it easier
	if (head(a).value.integer < 0 && head(b).value.integer < 0)
	{
		head(a).value.integer = std::abs(head(a).value.integer);
		head(b).value.integer = std::abs(head(b).value.integer);
	} // If b is negative then just pass to add func
	else if (head(a).value.integer < 0)
	{
		head(a).value.integer = std::abs(head(a).value.integer);
		
		// -a - b = -(a + b)
		Atom result = add_bignums(b, a);
		head(result).value.integer *= -1;

		return result;
	}
	else if (head(b).value.integer < 0)
	{
		head(b).value.integer = std::abs(head(b).value.integer);
		return add_bignums(a, b);
	}
	
	// a - b = -(b - a) & easier to determine a - b when a > b
	if (bignum_less(a, b))
	{
		Atom result = subtract_bignums(b, a);
		head(result).value.integer *= -1;

		return result;
	}
	
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

	result.type = Atom::BIGNUM;
	return result;
}

Atom multiply_bignums(Atom a, Atom b)
{
	bool negative = false;

	// If both are negative, cancel out signs
	if (head(a).value.integer < 0 && head(b).value.integer < 0)
	{
		head(a).value.integer = std::abs(head(a).value.integer);
		head(b).value.integer = std::abs(head(b).value.integer);
	} // If b is negative then just pass to add func
	else if (head(a).value.integer < 0)
	{
		head(a).value.integer = std::abs(head(a).value.integer);

		negative = true;
	}
	else if (head(b).value.integer < 0)
	{
		head(b).value.integer = std::abs(head(b).value.integer);
		
		negative = true;
	}

	Atom result = int_to_bignum(0);

	// Loop through digits in b
	int depth = 0;
	while (!nullp(b))
	{
		Atom high, low;
		split_bignum(copy_list(b), 1, &low, &high);

		Atom copy = copy_list(a);
		int multicand = head(high).value.integer;
		int index = depth;
		while (!nullp(copy))
		{
			Atom h, l;

			int split_at = std::min(bignum_length(copy), 8);

			split_bignum(copy_list(copy), split_at, &l, &h);
			Atom prod = int_to_bignum(head(h).value.integer * multicand);
			prod = shift_bignum(prod, index);

			result = add_bignums(result, prod);

			copy = l;
			index += split_at;
		}

		b = low;
		depth++;
	}

	if (negative) head(result).value.integer *= -1;

	return result;
}

Atom karatsuba_bignums(Atom a, Atom b)
{
	bool negative = false;

	// If both are negative, cancel out signs
	if (head(a).value.integer < 0 && head(b).value.integer < 0)
	{
		head(a).value.integer = std::abs(head(a).value.integer);
		head(b).value.integer = std::abs(head(b).value.integer);
	} // If b is negative then just pass to add func
	else if (head(a).value.integer < 0)
	{
		head(a).value.integer = std::abs(head(a).value.integer);

		negative = true;
	}
	else if (head(b).value.integer < 0)
	{
		head(b).value.integer = std::abs(head(b).value.integer);

		negative = true;
	}

	int lengths[2] = { bignum_length(copy_list(a)), bignum_length(copy_list(b)) };

	if (lengths[0] < 2 || lengths[1] < 2)
	{
		return multiply_bignums(a, b);
	}

	int middle = std::max(lengths[0], lengths[1]);
	middle /= 2;
	middle = std::min(std::min((lengths[0] - 1), (lengths[1] - 1)), middle);

	Atom low1, low2, high1, high2;
	split_bignum(copy_list(a), middle, &low1, &high1);
	split_bignum(copy_list(b), middle, &low2, &high2);

	// This looks quite terrifying, but it's just acz^2+bcz+adz+bd
	// Where low1 = a, high1 = b, low2 = c, high2 = d, z = 10^middle
	Atom result = add_bignums(shift_bignum(karatsuba_bignums(low1, low2), middle * 2),
		add_bignums(shift_bignum(karatsuba_bignums(high1, low2), middle), add_bignums(shift_bignum(karatsuba_bignums(low1, high2), middle),
			karatsuba_bignums(high1, high2))));

	if (negative) head(result).value.integer *= -1;

	return result;
}

// Uses long division
Atom divide_bignums(Atom dividend, Atom divisor)
{
	Atom quotient = int_to_bignum(0);

	// While the dividend is still larger than the divisor
	while (bignum_gore(dividend, divisor))
	{
		Atom shifted_divisor = copy_list(divisor);
		Atom temp_divisor = shift_bignum(copy_list(shifted_divisor));
		int shift = 0;
		
		// Shift the divisor until it can't be without being larger than the dividend
		// Ex. in 100/3, 3 -> 30
		while (bignum_gore(dividend, temp_divisor))
		{
			shifted_divisor = copy_list(temp_divisor);

			temp_divisor = shift_bignum(temp_divisor);
			shift++;
		}

		// Subtract dividend by shifted divisor until can't without divisor being negative
		while (bignum_gore(dividend, shifted_divisor))
		{
			dividend = subtract_bignums(dividend, copy_list(shifted_divisor));
			// Increase quotient each time dividend subtracted by 10^shift
			quotient = add_bignums(quotient, shift_bignum(int_to_bignum(1), shift));
		}
	}

	// Return quotient, and dividend which is now remainder
	return cons(quotient, dividend);
}

bool bignum_less(Atom a, Atom b)
{
	bool negative[2] = { (head(a).value.integer < 0), (head(b).value.integer < 0) };
	bool bothNegative;
	
	int a_val = head(a).value.integer;
	int b_val = head(b).value.integer;

	// If one is negative and the other isn't, it definitely doesn't have a higher value
	if (negative[0] && !negative[1]) return true;
	if (!negative[0] && negative[1]) return false;
	// If both are negative length means the opposite
	bothNegative = negative[0] && negative[1];

	// Length is a good indicator of size
	if (bignum_length(a) < bignum_length(b)) return !bothNegative;
	if (bignum_length(a) > bignum_length(b)) return bothNegative;

	a = tail(a);
	b = tail(b);

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

bool bignum_gore (Atom a, Atom b)
{
	return !bignum_less(a, b);
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
	Atom result = null;
	
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

	result.type = Atom::BIGNUM;

	return result;
}

void split_bignum(Atom bignum, int middle, Atom* low, Atom* high)
{
	int skip = bignum_length(bignum) - middle;

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
			else offset = 9 - middle;

			i += middle;
		}

		bignum = tail(bignum);
	}
	
	int a = (int)pow(10, 9 - offset);
	int b = (int)pow(10, offset);

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

	low->type = Atom::BIGNUM;
}