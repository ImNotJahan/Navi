#pragma once
#include "data.h"

// Return head value in pair
Error function_head(Atom args, Atom* result);
// Return tail value in pair
Error function_tail(Atom args, Atom* result);
// Create pair from head and tail value
Error function_pair(Atom args, Atom* result);

// Arithmetic

Error function_add(Atom args, Atom* result);
Error function_subtract(Atom args, Atom* result);
Error function_multiply(Atom args, Atom* result);
Error function_divide(Atom args, Atom* result);


// Return type of arg as symbol
Error function_type(Atom args, Atom* result);

// Comparisons

Error function_eq(Atom args, Atom* result);
Error function_less(Atom args, Atom* result);
Error function_and(Atom args, Atom* result);
Error function_or(Atom args, Atom* result);


// Apply args to func
Error function_apply(Atom args, Atom* result);

// Print expr to console
Error function_say(Atom args, Atom* result);
// Listen for input from console
Error function_listen(Atom args, Atom* result);

// Create ratio
Error function_ratio(Atom args, Atom* result);
// Covert other number type to int
Error function_int(Atom args, Atom* result);
// Convert other number type to float
Error function_float(Atom args, Atom* result);
// Convert other types to string
Error function_string(Atom args, Atom* result);
// Convert other types to bignum
Error function_bignum(Atom args, Atom* result);

// Return numerator of ratio
Error function_numerator(Atom args, Atom* result);
// Return denominator of ratio
Error function_denominator(Atom args, Atom* result);

// Throw error
Error function_error(Atom args, Atom* result);

// Bit shifts number
Error function_shift(Atom args, Atom* result);

#define is_number(a) (a.type == Atom::INTEGER || a.type == Atom::FLOAT || a.type == Atom::BIGNUM)

// Return if a and b both are type t
#define both_type(a, b, t) (a.type == Atom::t && b.type == Atom::t)
// Return if a and b have same value of type t
#define type_eq(a, b, t) (a.value.t == b.value.t) ? true_ : false_

// Make sure there is correct number of arguments
#define check_args(args, a, from) if (list_length(args) != a) return ARGNUM(from)

// Return atom true or false instead of cpp true or false
#define tf ? true_ : false_

// Throw error if a is not of type t
#define check_type(a, t, ts, f) if (a.type != Atom::t) return Error{ Error::TYPE, "Expected " ts, f }