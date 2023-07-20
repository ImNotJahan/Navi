#pragma once

// Makes pair from head and tail
Atom construct(Atom head, Atom tail);
// Shorthand for construct
#define cons(head, tail) construct(head, tail)

Atom make_int(long num);
Atom make_symbol(std::string* str);
Atom make_character(char character);
Error make_character(std::string character, Atom* result); // For escape characters
Atom make_float(float str);
Atom make_function(function func);
Error make_closure(Atom environment, Atom args, Atom body, Atom* result);
Error make_string(const std::string str, Atom* result);
Atom make_ratio(int numerator, int denominator);
Atom make_bignum(std::string number);

// Checks if expr is proper list
bool listp(Atom expr);

int list_length(Atom list);
Atom copy_list(Atom list);
Atom list_get(Atom list, int index);
void list_set(Atom list, int index, Atom value);
void list_reverse(Atom* list);

std::string to_string(Atom str);

// Function call info to be pushed to stack
Atom make_frame(Atom parent, Atom environment, Atom pending);

static Atom symbol_table = null;