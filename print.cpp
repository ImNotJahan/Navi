#include "lisp.h"
#include <iostream>
#include "data.h"

void print_expr(Atom atom)
{
	switch (atom.type)
	{
		case Atom::NULL_:
			std::cout << "NULL";
			break;

		case Atom::PAIR:
			std::cout << "(";

			print_expr(head(atom));
			atom = tail(atom);

			while (!nullp(atom))
			{
				if (atom.type == Atom::PAIR)
				{
					std::cout << " ";

					print_expr(head(atom));
					atom = tail(atom);
				}
				else
				{
					std::cout << " . ";

					print_expr(atom);
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

		case Atom::MACRO:
			std::cout << "MACRO";
			break;

		case Atom::STRING:
			while (!nullp(atom))
			{
				std::cout << head(atom).value.character;
				atom = tail(atom);
			}
			
			break;

		case Atom::RATIO:
			std::cout << atom.value.ratio.numerator << " / " << atom.value.ratio.denominator;
			break;

		case Atom::BIGNUM: // add bignum as well
			break;
	}
}

void print_err(Error err)
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

		default:
			std::cout << "Unkown error";
			break;
	}
	
	if (err.from != "") std::cout << " - " << err.from;
	if (err.details != "") std::cout << " - " << err.details;
}