#include "../include/main.h"
#include "../include/data.h"
#include "../include/garbage_collection.h"

Allocation* global_allocations = NULL;

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
			delete alloc;
		}
		else
		{
			pointer = &alloc->next;
		}
	}

	/* Clear marks */
	alloc = global_allocations;
	while (alloc != NULL)
	{
		alloc->mark = 0;
		alloc = alloc->next;
	}
}

Atom construct(Atom head, Atom tail)
{
	Allocation* alloc = new Allocation();
	Atom pair;

	alloc->mark = 0;
	alloc->next = global_allocations;
	global_allocations = alloc;

	pair.type = Atom::PAIR;
	pair.value.pair = &alloc->pair;

	head(pair) = head;
	tail(pair) = tail;

	return pair;
}

Atom make_int(long number)
{
	Atom atom;
	atom.type = Atom::INTEGER;
	atom.value.integer = number;

	return atom;
}

Atom make_symbol(const std::string* str)
{
	Atom atom, pair;

	pair = symbol_table;
	while (!nullp(pair))
	{
		atom = head(pair);
		if (*atom.value.symbol == *str) return atom;

		pair = tail(pair);
	}

	atom.type = Atom::SYMBOL;
	atom.value.symbol = str;

	symbol_table = construct(atom, symbol_table);

	return atom;
}

Atom make_character(char character)
{
	Atom atom;
	atom.type = Atom::CHARACTER;
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
		else if (character == "\\s") *result = make_character('\\'); // just a backlash, for some reason \\\\ kept having errors
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

	Atom atom = Atom{ Atom::STRING };
	atom.value.pair = pair.value.pair;

	*result = atom;

	return NOERR;
}

Atom make_ratio(int numerator, int denominator)
{
	Atom atom = Atom{ Atom::RATIO };
	atom.value.ratio.numerator = numerator;
	atom.value.ratio.denominator = denominator;

	return atom;
}

Atom make_bignum() // Implement this
{
	Atom atom = Atom{ Atom::BIGNUM };


	return atom;
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