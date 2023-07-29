#include "../include/main.h"
#include "../include/functions.h"
#include "../include/library.h"
#include "../include/evaluate.h"

#include <iostream>
#include "../include/garbage_collection.h"

int main(int argc, char** argv)
{
	std::string library_path = "navi/library.navi";
	std::string program_path = "";

	// Starts at 1 as the file (navi.exe) counts as one
	// Also increments by two since arguments and values come in pairs
	for (int i = 1; i < argc; i += 2)
	{
		// There should be a value to each argument
		if (argc < i + 1)
		{
			std::cout << "Incorrect arguments";
			return 1;
		}

		if (strcmp(argv[i], "--library-path") == 0) library_path = argv[i + 1];
		else if (strcmp(argv[i], "--program") == 0) program_path = argv[i + 1];
		else
		{
			std::cout << "Unknown argument";
			return 2;
		}
	}

	std::string input;

	Error err;
	Atom env;
	env = env_create(null);

	set_default_environment(&env);

	err = interpret_file(env, library_path, LogLevel::ERROR_ONLY);

	if (err.type && err.type != Error::EMPTY)
	{
		say_err(err);
		std::cout << std::endl;
	}

	// If a program is specified, run it then exit
	if (program_path != "")
	{
		err = interpret_file(env, program_path, LogLevel::ERROR_ONLY);

		if (err.type && err.type != Error::EMPTY) say_err(err);
		return 0;
	}

	// Otherwise start REPL
	std::cout << "> ";
	std::getline(std::cin, input);

	// End REPL if no input
	while (input != "")
	{
		Atom expr, result;

		err = read_expr(input, &input, &expr);

		if (!err.type) err = evaluate_expr(expr, env, &result);

		if (!err.type)
		{	
			// for optional top parenthesis
			if (result.type == Atom::FUNCTION && result.value.func->fixed_arg_num)
			{
				Atom args = null;
				
				for (int i = 0; i < result.value.func->arg_num; i++)
				{
					Atom arg, temp;

					err = read_expr(input, &input, &temp);

					mark(args); // since evaluate_expr will call collect, have to mark args

					if (!err.type) err = evaluate_expr(temp, env, &arg);
					
					args = cons(temp, args);

					if (err.type)
					{
						say_err(err);
						break;
					}
				}
				
				if (!err.type)
				{
					list_reverse(&args);
					Atom temp;

					result.value.func->func(args, &temp);
					say_expr(temp);
				}
			}
			else say_expr(result);
		}
		else say_err(err);

		if(err.type != Error::EMPTY) std::cout << std::endl;

		std::cout << "> ";
		std::getline(std::cin, input);
	}

	return 0;
}

void set_default_environment(Atom* env)
{
	env_set(*env, sym("HEAD"), make_function(function_head, 1));
	env_set(*env, sym("TAIL"), make_function(function_tail, 1));
	env_set(*env, sym("PAIR"), make_function(function_pair, 2));
	env_set(*env, sym("+"), make_function(function_add, 2));
	env_set(*env, sym("-"), make_function(function_subtract, 2));
	env_set(*env, sym("*"), make_function(function_multiply, 2));
	env_set(*env, sym("/"), make_function(function_divide, 2));
	env_set(*env, sym("TYPE_OF"), make_function(function_type, 1));
	env_set(*env, sym("="), make_function(function_eq, 2));
	env_set(*env, sym("<"), make_function(function_less, 2));
	env_set(*env, sym("APPLY"), make_function(function_apply));
	env_set(*env, sym("SAY"), make_function(function_say, 1));
	env_set(*env, sym("INT"), make_function(function_int, 1));
	env_set(*env, sym("FLOAT"), make_function(function_float, 1));
	env_set(*env, sym("RATIO"), make_function(function_ratio, 2));
	env_set(*env, sym("ERROR"), make_function(function_error));
	env_set(*env, sym("STRING"), make_function(function_string, 1));
	env_set(*env, sym("BIGNUM"), make_function(function_bignum, 1));
	env_set(*env, sym("LISTEN"), make_function(function_listen, 0));
	env_set(*env, sym("SHIFT"), make_function(function_shift));
	env_set(*env, sym("REMAINDER"), make_function(function_remainder, 2));
}