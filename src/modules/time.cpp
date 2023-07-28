#include "../../include/main.h"
#include "../../include/data.h"
#include "../../include/modules/time.h"

#include <time.h>

Error load_time(Atom env)
{
	env_set(env, sym("UNIX_TIME"), make_function(function_time));

	return NOERR;
}

Error function_time(Atom args, Atom* result)
{
	int arg_length = list_length(args);

	if (arg_length != -1) return ARGNUM("UNIX_TIME");
	
	Atom num;
	num = construct(make_int(1), construct(make_int(time(0) % 1000000000), null));
	num.type = Atom::BIGNUM;

	*result = num;

	return NOERR;
}