#include "../include/main.h"
#include "../include/data.h"

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
	switch (index)
	{
		case 0:
			return head(list);

		case 1:
			return head(tail(list));

		case 2:
			return head(tail(tail(list)));

		case 3:
			return head(tail(tail(tail(list))));

		case 4:
			return head(tail(tail(tail(tail(list)))));

		case 5:
			return head(tail(tail(tail(tail(tail(list))))));

		case 6:
			return head(tail(tail(tail(tail(tail(tail(list)))))));
	}

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
	switch (index)
	{
		case 0:
			head(list) = value;
			break;

		case 1:
			head(tail(list)) = value;
			break;

		case 2:
			head(tail(tail(list))) = value;
			break;

		case 3:
			head(tail(tail(tail(list)))) = value;
			break;

		case 4:
			head(tail(tail(tail(tail(list))))) = value;
			break;

		case 5:
			head(tail(tail(tail(tail(tail(list)))))) = value;
			break;

		case 6:
			head(tail(tail(tail(tail(tail(tail(list))))))) = value;
			break;

		default:
			while (index-- != 0) list = tail(list);
			head(list) = value;

			break;
	}
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

bool compare_lists(Atom l1, Atom l2)
{
	l1.type = Atom::PAIR;
	l2.type = Atom::PAIR;

	if (!(listp(l1) && listp(l2))) return false;

	if (list_length(l1) != list_length(l2)) return false;

	while (!nullp(l1))
	{
		Atom v1 = head(l1);
		Atom v2 = head(l2);

		if (v1.type != v2.type) return false;

		switch (v1.type)
		{
			case Atom::INTEGER:
				if (v1.value.integer != v2.value.integer) return false;
				break;

			case Atom::FLOAT:
				if (v1.value.float_ != v2.value.float_) return false;
				break;

			case Atom::CHARACTER:
				if (v1.value.character != v2.value.character) return false;
				break;

			case Atom::BOOLEAN:
				if (v1.value.boolean != v2.value.boolean) return false;
				break;

			case Atom::SYMBOL:
				if (v1.value.symbol != v2.value.symbol) return false;
				break;

			case Atom::RATIO:
				if (v1.value.ratio.denominator != v2.value.ratio.denominator) return false;
				if (v1.value.ratio.numerator != v2.value.ratio.numerator) return false;
				break;

			case Atom::PAIR:
			case Atom::BIGNUM:
			case Atom::STRING:
				if (!compare_lists(v1, v2)) return false;
				break;
		}

		l1 = tail(l1);
		l2 = tail(l2);
	}

	return true;
}