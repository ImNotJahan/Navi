#include "../include/main.h"
#include "../include/data.h"
#include "../include/garbage_collection.h"
#include "../include/strings.h"
#include "../include/numbers.h"

Allocation* global_allocations = NULL;

static size_t bytesAllocated = 0;
static size_t nextCollection = 1024 * 1024;

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
	
	nextCollection = bytesAllocated * 1.15;
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
		else return Error{ Error::TYPE, "Character expected", "MAKE_CHARACTER" };
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

Atom make_function(function func)
{
	Atom atom = Atom{ Atom::FUNCTION };
	atom.value.func = func;

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

Error make_string(Atom characters, Atom* result)
{	
	characters.type = Atom::STRING;
	*result = characters;

	return NOERR;
}

Atom make_ratio(int numerator, int denominator)
{
	Atom atom = Atom{ Atom::RATIO };
	atom.value.ratio.numerator = numerator;
	atom.value.ratio.denominator = denominator;

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

	if (negative) number = '-' + number; // Re-add negative sign

	int decimal_location = first_char(number, ".");

	if(decimal_location != -1) number[decimal_location] = '9';

	for (int i = number.length(); i > 0;)
	{
		int len = std::min(9, i);
		int ind = i - len;

		int num = stol(number.substr(ind, len));

		atom = cons(make_int(num), atom);

		i -= len;
	}

	atom = cons(make_int(decimal_location), atom);

	atom.type = Atom::BIGNUM;
	return atom;
}

int bignum_length(Atom bignum)
{
	return (list_length(bignum) - 2) * 9 + int_length(list_get(bignum, 1).value.integer);
}

bool listp(Atom expr)
{
	while (!nullp(expr))
	{
		if (expr.type != Atom::PAIR) return false;
		expr = tail(expr);
	}
	
	return true;
}

int list_length(Atom list)
{
	if (list.type != Atom::PAIR) return -1;

	int length = 1;
	Atom rest = tail(list);

	while (!nullp(rest))
	{
		rest = tail(rest);
		length++;
	}

	return length;
}

Atom copy_list(Atom list)
{
	Atom atom, pair;

	if (nullp(list)) return null;

	atom = cons(head(list), null);
	pair = atom;

	list = tail(list);

	while (!nullp(list))
	{
		tail(pair) = cons(head(list), null);
		pair = tail(pair);
		list = tail(list);
	}

	return atom;
}

Atom list_get(Atom list, int index)
{
	while (index-- != 0) list = tail(list);
	return head(list);
}

bool list_of_type_p(Atom list, Atom type)
{
	while (!nullp(list))
	{
		if (head(list).type != type.type) return false;
		list = tail(list);
	}

	return true;
}

void list_set(Atom list, int index, Atom value)
{
	while (index-- != 0) list = tail(list);
	head(list) = value;
}

void list_reverse(Atom* list)
{
	Atom back = null;

	while (!nullp((*list)))
	{
		Atom pair = tail((*list));
		tail((*list)) = back;

		back = *list;
		*list = pair;
	}

	*list = back;
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