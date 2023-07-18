#pragma once
#include <functional>
#include "data.h"

Error arithmetic(const std::function<float(float, float)>& f, Atom args, Atom* result);

Error function_head(Atom args, Atom* result);
Error function_tail(Atom args, Atom* result);
Error function_cons(Atom args, Atom* result);

Error function_add(Atom args, Atom* result);
Error function_subtract(Atom args, Atom* result);
Error function_multiply(Atom args, Atom* result);
Error function_divide(Atom args, Atom* result);

Error function_type(Atom args, Atom* result);

Error function_eq(Atom args, Atom* result);
Error function_less(Atom args, Atom* result);
Error function_and(Atom args, Atom* result);
Error function_or(Atom args, Atom* result);

Error function_apply(Atom args, Atom* result);
Error function_print(Atom args, Atom* result);

// Create ratio
Error function_ratio(Atom args, Atom* result);
// Covert other number type to int
Error function_int(Atom args, Atom* result);
// Convert other number type to float
Error function_float(Atom args, Atom* result);

Error function_numerator(Atom args, Atom* result);
Error function_denominator(Atom args, Atom* result);

#define is_number(a) (a.type == Atom::INTEGER || a.type == Atom::FLOAT)

#define both_type(a, b, t) (a.type == Atom::t && b.type == Atom::t)
#define type_eq(a, b, t) (a.value.t == b.value.t) ? true_ : false_

#define check_args(args, a) if (list_length(args) != a) return ARGNUM

#define tf ? true_ : false_