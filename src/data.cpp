#include "../include/main.h"
#include "../include/garbage_collection.h"
#include "../include/data.h"

Atom construct(Atom head, Atom tail)
{
	//Allocation* allocation = new Allocation();
	Atom pair;
	/*
	allocation->mark = 0;
	allocation->next = global_allocations;
	global_allocations = allocation;
	*/
	pair.type = Atom::PAIR;
	pair.value.pair = new Pair{ {head, tail} };

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
		else if (pair.type != Atom::PAIR || head(pair).type != Atom::SYMBOL) 
			return Error{ Error::TYPE, "Pair containing symbol symbol expected" };

		pair = tail(pair);
	}

	*result = cons(environment, cons(args, body));
	result->type = Atom::CLOSURE;

	return NOERR;
}

Atom make_string(const std::string str)
{
	Atom pair = null;

	for (int i = str.length() - 1; i >= 0; i--)
	{
		pair = cons(make_character(str[i]), pair);
	}

	Atom atom = Atom{ Atom::STRING };
	atom.value.pair = pair.value.pair;

	return atom;
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

// Data of expression to be pushed to stack
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