#include "lisp.h"
#include "data.h"

Error evaluate_expr(Atom expr, Atom environment, Atom* result)
{
	Atom operation, args, pair;
	Error err;

	// Symbols will evaluate to their value
	if (expr.type == Atom::SYMBOL) return env_get(environment, expr, result);
	// And literals will evaluate to themselves
	else if (expr.type != Atom::PAIR)
	{
		*result = expr;
		return NOERR;
	}

	if (!listp(expr)) return Error{ Error::SYNTAX, "Improper list" };

	// Non-quoted lists are evaluated as function calls

	operation = head(expr);
	args = tail(expr);

	if (operation.type == Atom::SYMBOL)
	{
		if (*operation.value.symbol == "QUOTE")
		{
			if (list_length(args) != 1) return ARGNUM;

			*result = head(args);
			return NOERR;
		}
		else if (*operation.value.symbol == "SET")
		{
			Atom symbol, value;

			if (list_length(args) < 2) return ARGNUM;

			symbol = head(args);
			if (symbol.type == Atom::PAIR)
			{
				err = make_closure(environment, tail(symbol), tail(args), &value);
				symbol = head(symbol);

				if (symbol.type != Atom::SYMBOL) 
					return Error{ Error::TYPE, "Expected symbol", "SET"};
			}
			else if (symbol.type == Atom::SYMBOL)
			{
				if (list_length(args) != 2) return ARGNUM;
				err = evaluate_expr(head(tail(args)), environment, &value);
			}
			else return Error{ Error::TYPE, "Expected symbol or pair", "SET"};

			if (err.type) return err;

			*result = symbol;
			return env_set(environment, symbol, value);
		}
		else if (*operation.value.symbol == "LAMBDA")
		{
			if (list_length(args) < 2) return ARGNUM;

			return make_closure(environment, head(args), tail(args), result);
		}
		else if (*operation.value.symbol == "IF")
		{
			Atom condition, value;

			if (list_length(args) != 3) return ARGNUM;

			err = evaluate_expr(head(args), environment, &condition);
			if (err.type) return err;

			if (condition.type != Atom::BOOLEAN) return Error{ Error::TYPE, "Expected bool", "IF"};

			value = condition.value.boolean ?
				head(tail(args)) : head(tail(tail(args)));

			return evaluate_expr(value, environment, result);
		}
		else if (*operation.value.symbol == "MACRO")
		{
			Atom name, macro;
			Error err;

			if (list_length(args) < 2) return ARGNUM;

			if (head(args).type != Atom::PAIR) return Error{ Error::SYNTAX };

			name = head(head(args));
			if (name.type != Atom::SYMBOL)
				return Error{ Error::TYPE, "Expected symbol for name", "MACRO"};

			err = make_closure(environment, tail(head(args)), tail(args), &macro);

			if (err.type) return err;

			macro.type = Atom::MACRO;
			*result = name;

			return env_set(environment, name, macro);
		}
	}
	
	// Evaluate operator

	err = evaluate_expr(operation, environment, &operation);
	if (err.type) return err;

	// For macros
	if (operation.type == Atom::MACRO)
	{
		Atom expansion;
		operation.type = Atom::CLOSURE;
		err = apply(operation, args, &expansion);

		if (err.type) return err;

		return evaluate_expr(expansion, environment, result);
	}

	// Evaluate arguments

	args = copy_list(args); // copy list incase needed in future
	pair = args;

	while (!nullp(pair))
	{
		err = evaluate_expr(head(pair), environment, &head(pair));

		if (err.type) return err;

		pair = tail(pair);
	}

	return apply(operation, args, result);
}

Error apply(Atom func, Atom args, Atom* result)
{
	if (func.type == Atom::FUNCTION) return (*func.value.func)(args, result);
	else if (func.type != Atom::CLOSURE)
		return Error{ Error::TYPE, "Expected function", "APPLY"};

	Atom environment, arg_names, body;
	environment = env_create(head(func));
	arg_names = head(tail(func));
	body = tail(tail(func));

	// Bind arguments to symbols in closure
	while (!nullp(arg_names))
	{
		// For variadic functions
		if (arg_names.type == Atom::SYMBOL)
		{
			env_set(environment, arg_names, args);
			args = null;
			break;
		}

		if (nullp(args)) return ARGNUM;

		env_set(environment, head(arg_names), head(args));

		arg_names = tail(arg_names);
		args = tail(args);
	}

	if (!nullp(args)) return ARGNUM;

	// Evaluate closure body
	while (!nullp(body))
	{
		Error err = evaluate_expr(head(body), environment, result);

		if (err.type) return err;

		body = tail(body);
	}

	return NOERR;
}