#include "../include/main.h"
#include "../include/data.h"

Atom env_create(Atom parent)
{
	return cons(parent, null);
}

Error env_get(Atom environment, Atom symbol, Atom* result)
{
	Atom parent = head(environment);
	Atom bindings = tail(environment);
	
	while (!nullp(bindings))
	{
		Atom binding = head(bindings);

		// If correct symbol, head should equal symbol and tail value
		if (head(binding).value.symbol == symbol.value.symbol)
		{
			*result = tail(binding);
			return NOERR;
		}

		bindings = tail(bindings);
	}

	if (nullp(parent)) return Error{ Error::UNBOUND, ("No symbol " + *symbol.value.symbol)};

	return env_get(parent, symbol, result); // Tries in parent environment
}

Error env_set(Atom environment, Atom symbol, Atom value)
{
	Atom bindings = tail(environment);
	Atom binding = null;

	// If binding already exists in environment, give it new value
	while (!nullp(bindings))
	{
		binding = head(bindings);
		if (*head(binding).value.symbol == *symbol.value.symbol)
		{
			delete& tail(binding);
			tail(binding) = value;
			
			return NOERR;
		}

		bindings = tail(bindings);
	}

	// Otherwise create new binding
	binding = cons(symbol, value);
	tail(environment) = cons(binding, tail(environment));

	return NOERR;
}