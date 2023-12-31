#include "../include/main.h"
#include "../include/data.h"
#include "../include/garbage_collection.h"
#include "../include/strings.h"
#include "../include/numbers.h"

Allocation* global_allocations = NULL;

static const size_t COLLECTION_MIN = 1024 * 1024;
static size_t bytesAllocated = 0;
static size_t nextCollection = COLLECTION_MIN;

// Implementing here so global_allocations only has to be referenced in data.cpp
void collect()
{
	Allocation* alloc, ** pointer;

	mark(symbol_table);

	/* Free unmarked allocations */
	pointer = &global_allocations;
	while (*pointer != NULL)
	{
		alloc = *pointer;
		if (!alloc->mark)
		{
			*pointer = alloc->next;
			bytesAllocated -= sizeof(alloc);

			delete alloc;
		}
		else
		{
			alloc->mark = false;
			pointer = &alloc->next;
		}
	}
	
	nextCollection = std::max((size_t) (bytesAllocated * 1.15), COLLECTION_MIN);
}

size_t getBytesAllocated()
{
	return bytesAllocated;
}

size_t getNextCollection()
{
	return nextCollection;
}

Atom construct(Atom head, Atom tail)
{
	Allocation* alloc = new Allocation();
	Atom pair{ Atom::PAIR };

	alloc->mark = 0;
	alloc->next = global_allocations;
	global_allocations = alloc;

	pair.value.pair = &alloc->pair;

	head(pair) = head;
	tail(pair) = tail;

	bytesAllocated += sizeof(alloc);

	return pair;
}

Atom make_int(long number)
{
	Atom atom{ Atom::INTEGER };
	atom.value.integer = number;

	return atom;
}

Atom make_symbol(std::string* str)
{
	Atom atom, pair;

	pair = symbol_table;
	while (!nullp(pair))
	{
		atom = head(pair);
		if (*atom.value.symbol == *str)
		{
			delete str;
			return atom;
		}

		pair = tail(pair);
	}

	atom.type = Atom::SYMBOL;
	atom.value.symbol = str;
	
	symbol_table = construct(atom, symbol_table);

	return atom;
}

Atom make_character(char character)
{
	Atom atom{ Atom::CHARACTER };
	atom.value.character = character;

	return atom;
}

Error make_character(std::string character, Atom* result)
{
	if (character.length() > 1)
	{
		if (character == "\\n") *result = make_character('\n'); // newline
		else if (character == "\\t") *result = make_character('\t'); // tab
		else if (character == "\\b") *result = make_character('\b'); // backspace
		else if (character == "\\f") *result = make_character('\f'); // form feed (new page)
		else if (character == "\\r") *result = make_character('\r'); // carriage return (think typewritter)
		else if (character == "\\v") *result = make_character('\v'); // vertical whitespace
		else if (character == "\\s") *result = make_character(' '); // space
		else if (character == "\\\"") *result = make_character('"'); // "
		else if (character == "\\p") *result = make_character('\\'); // just a backlash, for some reason \\\\ kept having errors
		else *result = make_character(character[1]);
	}
	else *result = make_character(character[0]);

	return NOERR;
}

Atom make_float(float number)
{
	Atom atom = Atom{ Atom::FLOAT };
	atom.value.float_ = number;

	return atom;
}

FunctionData::FunctionData(function func, bool fixed_arg_num, int arg_num)
{
	this->func = func;
	this->fixed_arg_num = fixed_arg_num;
	this->arg_num = arg_num;
}

Atom make_function(function func)
{
	Atom atom = Atom{ Atom::FUNCTION };
	atom.value.func = new FunctionData(func, false, 0);

	return atom;
}

Atom make_function(function func, int arg_num)
{
	Atom atom = Atom{ Atom::FUNCTION };
	atom.value.func = new FunctionData(func, true, arg_num);

	return atom;
}

Error make_closure(Atom environment, Atom args, Atom body, Atom* result)
{
	Atom pair;

	if (!listp(body)) return Error{ Error::SYNTAX, "No closure body"};

	// Check argument names are all symbols
	pair = args;

	while (!nullp(pair))
	{
		if (pair.type == Atom::SYMBOL) break;
		else if (pair.type != Atom::PAIR) return Error{ Error::TYPE, "Pair expected", "MAKE_CLOSURE"};

		if(head(pair).type == Atom::SYMBOL)

		pair = tail(pair);
	}

	*result = cons(environment, cons(args, body));
	result->type = Atom::CLOSURE;

	return NOERR;
}

Error make_string(const std::string parameter_str, Atom* result)
{
	if (parameter_str == "")
	{
		*result = null;
		return NOERR;
	}

	Atom pair = null;
	std::string str = parameter_str;

	for (int i = str.length() - 1; i >= 0; i--)
	{
		if (i - 1 >= 0 && str[i - 1] == '\\')
		{
			Atom result;
			Error err = make_character(std::string() + str[i - 1] + str[i], &result);

			if (err.type) return err;

			pair = cons(result, pair);

			i--;
		}
		else
		{
			pair = cons(make_character(str[i]), pair);
		}
	}

	pair.type = Atom::STRING;
	*result = pair;

	return NOERR;
}

Atom make_ratio(Atom numerator, Atom denominator)
{
	Atom atom;
	atom = cons(numerator, denominator);

	atom.type = Atom::RATIO;
	return atom;
}

Atom make_bignum(std::string number)
{
	Atom atom = null;
	bool negative = false;

	// No one cares about + signs
	if (number[0] == '+') number.substr(1);
	// Temporarly remove negative sign so leading zeros can be removed
	else if (number[0] == '-')
	{
		number = number.substr(1);
		negative = true;
	}

	remove_starting_chars(number, "0"); // Remove leading zeros

	for (int i = number.length(); i > 0;)
	{
		int len = std::min(9, i);
		int ind = i - len;

		int num = stol(number.substr(ind, len));

		atom = cons(make_int(num), atom);

		i -= len;
	}

	// stol counts negative sign in input despite being signed result, so just add it in later
	if (negative)
	{
		head(atom).value.integer = head(atom).value.integer * -1;
	}

	atom.type = Atom::BIGNUM;
	return atom;
}

int bignum_length(Atom bignum)
{
	return (list_length(copy_list(bignum)) - 1) * 9 + int_length(head(copy_list(bignum)).value.integer);
}

Atom int_to_bignum(int integer)
{
	Atom bignum;
	bignum = cons(make_int(integer), null);
	bignum.type = Atom::BIGNUM;

	return bignum;
}

std::string to_string(Atom str)
{
	std::string temp = "";

	while (!nullp(str))
	{
		temp += head(str).value.character;
		str = tail(str);
	}

	return temp;
}

Atom make_frame(Atom parent, Atom environment, Atom pending)
{
	// (parent environment evaluated_operation (pending_arguments...) (evaluated_arguments...) (body...))
	return cons(parent, 
		cons(environment, 
		cons(null, // operation
		cons(pending, 
		cons(null, // arguments
		cons(null, // body
			null))))));
}