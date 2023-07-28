#include "../../include/main.h"
#include "../../include/data.h"
#include "../../include/modules/random.h"

#include <cstdlib>
#include <time.h>

int seed = 1;

Error load_random(Atom env)
{
	seed = time(0);
	srand(seed);

	env_set(env, sym("RANDOM"), make_function(function_random));
	env_set(env, sym("SET_RANDOM_SEED"), make_function(function_set_seed));
	env_set(env, sym("GET_RANDOM_SEED"), make_function(function_get_seed));

	return NOERR;
}

Error function_random(Atom args, Atom* result)
{
	int arg_length = list_length(args);

	if (arg_length == -1)
	{
		*result = make_int(std::rand());
	}
	else return ARGNUM("RANDOM");

	return NOERR;
}

Error function_set_seed(Atom args, Atom* result)
{
	int arg_length = list_length(args);

	if (arg_length != 1) return ARGNUM("SET_RANDOM_SEED");

	if (head(args).type != Atom::INTEGER) return Error{ Error::TYPE, "Expected integer for seed", "SET_RANDOM_SEED" };

	seed = head(args).value.integer;
	srand(seed);

	return NOERR;
}

Error function_get_seed(Atom args, Atom* result)
{
	if (!nullp(args)) return ARGNUM("GET_RANDOM_SEED");

	*result = make_int(seed);

	return NOERR;
}