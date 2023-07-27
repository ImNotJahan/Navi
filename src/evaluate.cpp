#include "../include/main.h"
#include "../include/evaluate.h"
#include "../include/data.h"
#include "../include/library.h"
#include "../include/garbage_collection.h"

Error evaluate_expr(Atom expr, Atom environment, Atom* result)
{
	Frame stack;
	stack.start = true;

	Error err = NOERR;

	do
	{
		if (getBytesAllocated() > getNextCollection())
		{
			mark(environment);
			mark(expr);
			mark_frame(stack);
			
			collect();
		}

		// Symbols will evaluate to their value
		if (expr.type == Atom::SYMBOL) err = env_get(environment, expr, result);
		// And literals will evaluate to themselves
		else if (expr.type != Atom::PAIR) *result = expr;
		else if (!listp(expr)) return Error{ Error::SYNTAX, "Improper list" };
		else
		{
			// Non-quoted lists are evaluated as function calls

			Atom operation = head(expr);
			Atom args = tail(expr);
			
			if (operation.type == Atom::SYMBOL)
			{
				// Special forms

				if (*operation.value.symbol == "QUOTE")
				{
					if (list_length(args) != 1) return ARGNUM("QUOTE");

					*result = head(args);
				}
				else if (*operation.value.symbol == "SET")
				{
					Atom symbol;

					if (list_length(args) < 2) return ARGNUM("SET");

					symbol = head(args);
					if (symbol.type == Atom::PAIR)
					{
						err = make_closure(environment, tail(symbol), tail(args), result);
						symbol = head(symbol);

						if (symbol.type != Atom::SYMBOL)
							return Error{ Error::TYPE, "Expected symbol", "SET" };

						env_set(environment, symbol, *result);
						*result = symbol;
					}
					else if (symbol.type == Atom::SYMBOL)
					{
						int arg_length = list_length(args);

						// (set x 10)
						if (arg_length == 2) expr = head(tail(args));
						// (set x to 10)
						else if (arg_length == 3) expr = head(tail(tail(args)));
						else return ARGNUM("SET");
						
						stack = make_frame(stack, environment, null);

						stack.evaluated_operation = operation;
						stack.evaluated_arguments = symbol;

						continue;
					}
					else return Error{ Error::TYPE, "Expected symbol or pair", "SET" };
				}
				else if (*operation.value.symbol == "CHANGE")
				{
					if (list_length(args) != 2) return ARGNUM("CHANGE");

					Atom symbol = head(args);

					if (symbol.type != Atom::SYMBOL) return Error{ Error::TYPE, "Expected symbol", "CHANGE" };

					stack = make_frame(stack, environment, null);
					stack.evaluated_operation = operation;
					stack.evaluated_arguments = symbol;

					expr = head(tail(args));

					continue;
				}
				else if (*operation.value.symbol == "LAMBDA")
				{
					if (list_length(args) < 2) return ARGNUM("LAMBDA");

					err = make_closure(environment, head(args), tail(args), result);
				}
				else if (*operation.value.symbol == "IF")
				{
					int arg_length = list_length(args);
					if (arg_length != 3 && arg_length != 5) return ARGNUM("IF");

					stack = make_frame(stack, environment, tail(args));
					stack.evaluated_operation = operation;

					expr = head(args);
					continue;
				}
				else if (*operation.value.symbol == "EXPAND")
				{
					Atom name, macro;

					if (list_length(args) < 2) return ARGNUM("EXPAND");

					if (head(args).type != Atom::PAIR) return Error{ Error::SYNTAX };

					name = head(head(args));
					if (name.type != Atom::SYMBOL)
						return Error{ Error::TYPE, "Expected symbol for name", "EXPAND" };

					err = make_closure(environment, tail(head(args)), tail(args), &macro);

					if (!err.type)
					{
						macro.type = Atom::EXPAND;
						*result = name;

						env_set(environment, name, macro);
					}
				}
				else if (*operation.value.symbol == "APPLY")
				{
					if (list_length(args) != 2) return ARGNUM("APPLY");

					stack = make_frame(stack, environment, tail(args));
					stack.evaluated_operation = operation;

					expr = head(args);
					continue;
				}
				else if (*operation.value.symbol == "LOAD")
				{
					int argLen = list_length(args);
					if (argLen < 1 || argLen > 2) return ARGNUM("LOAD");

					if (head(args).type != Atom::STRING) return Error{ Error::TYPE, "Expected string for file path", "LOAD" };

					std::string symbols_to_load = "*";

					if (argLen == 2)
					{
						if (head(tail(args)).type != Atom::STRING) return Error{ Error::TYPE, "Expected string for symbols to load", "LOAD" };
						symbols_to_load = to_string(head(tail(args)));
					}

					std::string path = to_string(head(args));
					err = interpret_file(environment, path, LogLevel::ERROR_ONLY, symbols_to_load);

					if(err.type != Error::EMPTY) return err;

					continue;
				}
				else if (*operation.value.symbol == "REFLECT")
				{
					if (list_length(args) != 1) return ARGNUM("REFLECT");

					Atom arg = head(args);

					if (arg.type == Atom::SYMBOL)
					{
						err = env_get(environment, arg, &arg);

						if (err.type) return err;
					}
					if (arg.type != Atom::STRING) return Error{ Error::TYPE, "Expected string", "REFLECT" };

					Atom op;

					std::string input = to_string(arg);
					err = read_expr(input, &input, &op);

					if (!err.type)
					{
						expr = op;
						continue;
					}
				}
				else goto push;
			}
			else if (operation.type == Atom::FUNCTION)
			{
				err = (*operation.value.func)(args, result);
			}
			else
			{
			push:
				// handle functions
				stack = make_frame(stack, environment, args);
				expr = operation;
				
				continue;
			}
		}
		
		if (stack.start) break;
		if (!err.type) err = evaluate_do_returning(&stack, &expr, &environment, result);
	} while (!err.type);
	
	mark(environment); 
	mark(expr); 
	mark_frame(stack); 
	mark(*result); 
	collect();

	return err;
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

		if (nullp(args)) return ARGNUM("APPLY");

		env_set(environment, head(arg_names), head(args));

		arg_names = tail(arg_names);
		args = tail(args);
	}

	if (!nullp(args)) return ARGNUM("APPLY");

	// Evaluate closure body
	while (!nullp(body))
	{
		Error err = evaluate_expr(head(body), environment, result);

		if (err.type) return err;

		body = tail(body);
	}

	return NOERR;
}

Error evaluate_do_execution(Frame* stack, Atom* expr, Atom* environment)
{
	Atom body;
	
	*environment = stack->environment;
	body = stack->body;
	
	*expr = head(body);
	body = tail(body);
	
	if (nullp(body)) *stack = *stack->parent; // pop stack as nothing left to execute
	else stack->body = body;

	return NOERR;
}

Error evaluate_do_binding(Frame* stack, Atom* expr, Atom* environment)
{
	Atom operation, args, arg_names, body;
	
	body = stack->body;
	if (!nullp(body)) return evaluate_do_execution(stack, expr, environment);
	
	operation = stack->evaluated_operation;
	args = stack->evaluated_arguments;
	
	*environment = env_create(head(operation));
	arg_names = head(tail(operation));
	body = tail(tail(operation));

	// Update stack frame
	stack->environment = *environment;
	stack->body = body;

	// Bind arguments
	while (!nullp(arg_names))
	{
		if (arg_names.type == Atom::SYMBOL)
		{
			env_set(*environment, arg_names, args);
			args = null;
			break;
		}

		if (nullp(args)) return ARGNUM("EVALUATE_DO_BINDING");

		env_set(*environment, head(arg_names), head(args));
		arg_names = tail(arg_names);
		args = tail(args);
	}

	if (!nullp(args)) return ARGNUM("EVALUATE_DO_BINDING");
	
	stack->evaluated_arguments = null; // finished binding args so clear from frame
	
	return evaluate_do_execution(stack, expr, environment);
}

Error evaluate_do_applying(Frame* stack, Atom* expr, Atom* environment, Atom* result)
{
	Atom operation, args;
	
	operation = stack->evaluated_operation;
	args = stack->evaluated_arguments;
	
	if (!nullp(args) && listp(args))
	{
		list_reverse(&args);
		stack->evaluated_arguments = args;
	}
	
	if (operation.type == Atom::SYMBOL)
	{
		if (*operation.value.symbol == "APPLY")
		{
			// Replace current frame
			*stack = *stack->parent;
			*stack = make_frame(*stack, *environment, null);

			operation = head(args);
			args = head(tail(args));

			if (!listp(args)) 
				return Error{ Error::SYNTAX, "Args not provided as list", "EVALUATE_DO_APPLYING"};

			stack->evaluated_operation = operation;
			stack->evaluated_arguments = args;
		}
	}

	if (operation.type == Atom::FUNCTION)
	{
		*stack = *stack->parent;
		
		*expr = cons(operation, args);

		return NOERR;
	}
	else if (operation.type != Atom::CLOSURE) 
		return Error{ Error::TYPE, "Function or closure expected", "EVALUATE_DO_APPLYING" };
	
	return evaluate_do_binding(stack, expr, environment);
}

Error evaluate_do_returning(Frame* stack, Atom* expr, Atom* environment, Atom* result)
{
	Atom operation, args, body;
	
	*environment = stack->environment;
	operation = stack->evaluated_operation;
	body = stack->body;
	
	if (!nullp(body)) // Still body to evaluate
		return evaluate_do_applying(stack, expr, environment, result);

	if (nullp(operation)) // Finished evaluating operator
	{
		operation = *result;
		stack->evaluated_operation = operation;
		
		if (operation.type == Atom::EXPAND)
		{
			// Don't evaluate macro arguments
			args = stack->pending_arguments;
			*stack = make_frame(*stack, *environment, null);

			operation.type = Atom::CLOSURE;

			stack->evaluated_operation = operation;
			stack->evaluated_arguments = args;

			return evaluate_do_binding(stack, expr, environment);
		}
	}
	else if (operation.type == Atom::SYMBOL)
	{
		if (*operation.value.symbol == "SET")
		{
			Atom symbol = stack->evaluated_arguments;

			env_set(*environment, symbol, *result);
			*stack = *stack->parent;
			*expr = cons(sym("QUOTE"), cons(symbol, null));

			return NOERR;
		}
		else if (*operation.value.symbol == "CHANGE")
		{
			Atom symbol = stack->evaluated_arguments;

			Error err = env_change(*environment, symbol, *result);
			*stack = *stack->parent;
			*expr = cons(sym("QUOTE"), cons(symbol, null));

			return err;
		}
		else if (*operation.value.symbol == "IF")
		{
			args = stack->pending_arguments;
			int arg_length = list_length(args);

			if ((*result).type != Atom::BOOLEAN) return Error{ Error::TYPE, "Expected bool", "IF" };

			Atom if_do, else_do;

			if (arg_length == 4)
			{
				if_do = head(tail(args));
				else_do = head(tail(tail(tail(args))));
			}
			else if (arg_length == 2)
			{
				if_do = head(args);
				else_do = head(tail(args));
			}
			else
			{
				return Error{ Error::ARGS, "Somehow have incorrect number of args (should not reach here)", "EVALUATE_DO_RETURNING IF" };
			}

			*expr = (*result).value.boolean ? if_do : else_do;
			*stack = *stack->parent;

			return NOERR;
		}
		else goto store_arg;
	}
	else if (operation.type == Atom::EXPAND)
	{
		*expr = *result;
		*stack = *stack->parent;

		return NOERR;
	}
	else
	{
	store_arg: // store evaluated arguments
		args = stack->evaluated_arguments;

		stack->evaluated_arguments = cons(*result, args);
	}
	
	args = stack->pending_arguments;

	if (nullp(args))
	{
		// no more arguments to evaluate
		return evaluate_do_applying(stack, expr, environment, result);
	}
	
	// evaluate next argument
	*expr = head(args);
	stack->pending_arguments = tail(args);

	return NOERR;
}