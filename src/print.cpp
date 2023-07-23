#include "../include/main.h"
#include <iostream>
#include "../include/data.h"
#include "../include/numbers.h"

void say_expr(Atom atom)
{
	switch (atom.type)
	{
		case Atom::NULL_:
			std::cout << "NULL";
			break;

		case Atom::PAIR:
			std::cout << "(";

			say_expr(head(atom));
			atom = tail(atom);

			while (!nullp(atom))
			{
				if (atom.type == Atom::PAIR)
				{
					std::cout << " ";

					say_expr(head(atom));
					atom = tail(atom);
				}
				else
				{
					std::cout << " . ";

					say_expr(atom);
					break;
				}
			}

			std::cout << ")";
			break;

		case Atom::SYMBOL:
			std::cout << *atom.value.symbol;
			break;

		case Atom::INTEGER:
			std::cout << atom.value.integer;
			break;

		case Atom::FLOAT:
			std::cout << atom.value.float_;
			break;

		case Atom::CHARACTER:
			std::cout << atom.value.character;
			break;

		case Atom::FUNCTION:
			std::cout << "BUILTIN_FUNCTION";
			break;

		case Atom::CLOSURE:
			std::cout << "CLOSURE";
			break;

		case Atom::BOOLEAN:
			std::cout << (atom.value.boolean ? "TRUE" : "FALSE");
			break;

		case Atom::EXPAND:
			std::cout << "EXPAND";
			break;

		case Atom::STRING:
			std::cout << to_string(atom);
			break;

		case Atom::RATIO:
			std::cout << atom.value.ratio.numerator << ":" << atom.value.ratio.denominator;
			break;

		case Atom::BIGNUM:
			std::string num = "";

			num += std::to_string(head(atom).value.integer);
			atom = tail(atom);

			while (!nullp(atom))
			{
				int nums = head(atom).value.integer;
				
				for (int i = int_length(nums) + 1; i < 10; i++) num += "0";

				num += std::to_string(nums);
				atom = tail(atom);
			}

			std::cout << num;

			break;
	}
}

void say_err(Error err)
{
	switch (err.type)
	{
		case Error::SYNTAX:
			std::cout << "Syntax error";
			break;

		case Error::UNBOUND:
			std::cout << "Unbound error";
			break;

		case Error::ARGS:
			std::cout << "Argument error";
			break;

		case Error::TYPE:
			std::cout << "Type error";
			break;

		case Error::EMPTY:
			if (err.details != "") std::cout << err.details;
			return;
			break;

		default:
			std::cout << "Unknown error";
			break;
	}
	
	if (err.from != "") std::cout << " - " << err.from;
	if (err.details != "") std::cout << " - " << err.details;
}