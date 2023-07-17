#include "lisp.h"
#include "functions.h"
#include "library.h"
#include <iostream>

int main()
{
	std::string input;

	Atom env;
	env = env_create(null);

	set_default_environment(&env);

	interpret_file(env, "library.navi", LogLevel::ERROR_ONLY);

	std::cout << "> ";
	std::getline(std::cin, input);

	while (input != "")
	{
		Error err;
		Atom expr, result;

		err = read_expr(input, &input, &expr);

		if (!err.type) err = evaluate_expr(expr, env, &result);

		if (!err.type) print_expr(result);
		else print_err(err);

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
	env_set(*env, sym("CONS"), make_function(function_cons));
	env_set(*env, sym("+"), make_function(function_add));
	env_set(*env, sym("-"), make_function(function_subtract));
	env_set(*env, sym("*"), make_function(function_multiply));
	env_set(*env, sym("/"), make_function(function_divide));
	env_set(*env, sym("TYPE"), make_function(function_type));
	env_set(*env, sym("="), make_function(function_eq));
	env_set(*env, sym("<"), make_function(function_less));
	env_set(*env, sym("AND"), make_function(function_and));
	env_set(*env, sym("OR"), make_function(function_or));
	env_set(*env, sym("APPLY"), make_function(function_apply));
	env_set(*env, sym("PRINT"), make_function(function_print));
}