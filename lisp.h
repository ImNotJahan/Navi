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
			print_expr in print.cpp
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
		MACRO,
		STRING,
		BIGNUM, // to support
		RATIO // to support
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

// car
#define head(p) (p.value.pair->atom[0])
// cdr
#define tail(p) (p.value.pair->atom[1])

#define nullp(atom) (atom.type == Atom::NULL_)

// Shorthand for make_symbol
#define sym(value) make_symbol(new std::string(value))

static const Atom null{ Atom::NULL_ };
static const Atom true_{ Atom::BOOLEAN, true };
static const Atom false_{ Atom::BOOLEAN, false };

void print_expr(Atom atom);

struct Error
{
	// Also update print_err in parser.cpp when adding error type
	enum
	{
		OK = 0,
		SYNTAX,
		UNBOUND,
		ARGS,
		TYPE
	} type;

	std::string details = "";
	std::string from = "";
};

#define NOERR Error{ Error::OK }
#define ARGNUM Error{ Error::ARGS, "Wrong number of arguments" }

// Reads and then sends expression to correct function
Error read_expr(std::string input, std::string* end, Atom* result);

void print_err(Error err);

Atom env_create(Atom parent);
// Searches environment and parent environments for symbol
Error env_get(Atom environment, Atom symbol, Atom* result);
// Sets symbol to value in environment
Error env_set(Atom environment, Atom symbol, Atom value);
Error evaluate_expr(Atom expr, Atom environment, Atom* result);

Error apply(Atom func, Atom args, Atom* result);

void set_default_environment(Atom* env);