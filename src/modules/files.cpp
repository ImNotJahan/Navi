#include "../../include/main.h"
#include "../../include/data.h"
#include "../../include/library.h"
#include "../../include/modules/files.h"

#include <fstream>
#include <iostream>

Error load_files(Atom env)
{
	env_set(env, sym("OPEN"), make_function(function_open, 1));

	return NOERR;
}

Error function_open(Atom args, Atom* result)
{
	if (list_length(args) != 1) ARGNUM("OPEN");

	Atom path = head(args);
	if (path.type != Atom::STRING) return Error{ Error::TYPE, "Expected string for path", "OPEN" };

	std::string contents = read_file(to_string(path));

	if(contents == "") return Error{ Error::SYNTAX, "File doesn't exist or is empty", "OPEN" };

	Atom str;
	Error err;
	err = make_string(contents, &str);

	if (err.type) return err;

	*result = str;
	
	return NOERR;
}