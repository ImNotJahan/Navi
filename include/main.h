#pragma once
#include <string>

struct Error;
struct Atom;

typedef Error (*function)(struct Atom args, struct Atom* result);

struct Ratio
{
	int numerator;
	int denominator;
};

struct Atom
{
	// I had to put _ infront of a few things since they were reserved keywords
	/* 
		When adding a type to this list, also update 
			function_type in functions.cpp
			say_expr in print.cpp
			parser.cpp
		And create a make_type in data.cpp
	*/
	enum
	{
		NULL_,
		INTEGER,
		SYMBOL,
		CHARACTER,
		PAIR,
		FLOAT,
		FUNCTION,
		CLOSURE,
		BOOLEAN,
		EXPAND,
		STRING,
		BIGNUM,
		RATIO
	} type;

	union
	{
		bool boolean;
		struct Pair* pair;
		const std::string* symbol;
		long integer;
		float float_;
		char character;
		function func;
		struct Ratio ratio;
	} value;
};

// Consists of a head atom and tail atom
struct Pair
{
	struct Atom atom[2];
};

// Get first value in pair. Equivalent to car
#define head(p) (p.value.pair->atom[0])
// Get second value in pair. Equivalent to cdr
#define tail(p) (p.value.pair->atom[1])

// Is it null?
#define nullp(atom) (atom.type == Atom::NULL_)

// Shorthand for make_symbol
#define sym(value) make_symbol(new std::string(value))

// Constants

static const Atom null{ Atom::NULL_ };
static const Atom true_{ Atom::BOOLEAN, true };
static const Atom false_{ Atom::BOOLEAN, false };


void say_expr(Atom atom);

struct Error
{
	// Also update say_err in parser.cpp when adding error type
	enum
	{
		OK = 0, // If no error
		SYNTAX,
		UNBOUND,
		ARGS,
		TYPE,
		EMPTY // Used for when there should be nothing printed, eg. comments & load
	} type;

	// Details about the error
	std::string details = "";
	// What part of the interpreter was running when the error occured
	std::string from = "";
};

static const Error NOERR = Error{ Error::OK };
#define ARGNUM(from) Error{ Error::ARGS, "Wrong number of arguments", from }

// Reads and then sends expression to correct function
Error read_expr(std::string input, std::string* end, Atom* result);

void say_err(Error err);

Atom env_create(Atom parent);
// Searches environment and parent environments for symbol
Error env_get(Atom environment, Atom symbol, Atom* result);
// Sets symbol to value in environment
Error env_set(Atom environment, Atom symbol, Atom value);
// Changes value of symbol in environment or parent environments
Error env_change(Atom environment, Atom symbol, Atom value);

// Runs func with arguments
Error apply(Atom func, Atom args, Atom* result);

// Registers all the built-in functions to the provided environment
void set_default_environment(Atom* env);