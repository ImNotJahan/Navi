#include "../include/main.h"
#include "../include/functions.h"
#include "../include/library.h"
#include "../include/evaluate.h"

#include <iostream>

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

	Atom env;
	env = env_create(null);

	set_default_environment(&env);

	interpret_file(env, library_path, LogLevel::ERROR_ONLY);

	// If a program is specified, run it then exit
	if (program_path != "")
	{
		interpret_file(env, program_path, LogLevel::ERROR_ONLY);

		return 0;
	}

	// Otherwise start REPL
	std::cout << "> ";
	std::getline(std::cin, input);

	// End REPL if no input
	while (input != "")
	{
		Error err;
		Atom expr, result;

		err = read_expr(input, &input, &expr);

		if (!err.type) err = evaluate_expr(expr, env, &result);

		if (!err.type) say_expr(result);
		else say_err(err);

		std::cout << std::endl;

		std::cout << "> ";
		std::getline(std::cin, input);
	}

	return 0;
}

void set_default_environment(Atom* env)
{
	env_set(*env, sym("HEAD"), make_function(function_head));
	env_set(*env, sym("TAIL"), make_function(function_tail));
	env_set(*env, sym("PAIR"), make_function(function_pair));
	env_set(*env, sym("+"), make_function(function_add));
	env_set(*env, sym("-"), make_function(function_subtract));
	env_set(*env, sym("*"), make_function(function_multiply));
	env_set(*env, sym("/"), make_function(function_divide));
	env_set(*env, sym("TYPE_OF"), make_function(function_type));
	env_set(*env, sym("="), make_function(function_eq));
	env_set(*env, sym("<"), make_function(function_less));
	env_set(*env, sym("AND"), make_function(function_and));
	env_set(*env, sym("OR"), make_function(function_or));
	env_set(*env, sym("APPLY"), make_function(function_apply));
	env_set(*env, sym("SAY"), make_function(function_say));
	env_set(*env, sym("INT"), make_function(function_int));
	env_set(*env, sym("FLOAT"), make_function(function_float));
	env_set(*env, sym("RATIO"), make_function(function_ratio));
	env_set(*env, sym("NUMERATOR"), make_function(function_numerator));
	env_set(*env, sym("DENOMINATOR"), make_function(function_denominator));
	env_set(*env, sym("ERROR"), make_function(function_error));
	env_set(*env, sym("STRING"), make_function(function_string));
	env_set(*env, sym("BIGNUM"), make_function(function_bignum));
}