#pragma once

// Makes pair from head and tail
Atom construct(Atom head, Atom tail);
// Shorthand for construct
#define cons(head, tail) construct(head, tail)

// Creates atom int from long
Atom make_int(long num);
// Creates symbol from string
Atom make_symbol(std::string* str);
// Creates character from character
Atom make_character(char character);
// Creates character from string of escape character
Error make_character(std::string character, Atom* result);
// Creates float from float
Atom make_float(float str);
// Creates function with variable number of arguments from function
Atom make_function(function func);
// Creates function with fixed number of arguments from function
Atom make_function(function func, int arg_num);
// Creates closure from environment with args args body body and sends to result
Error make_closure(Atom environment, Atom args, Atom body, Atom* result);
// Makes string from std::string
Error make_string(const std::string str, Atom* result);
// Makes ratio from parts
Atom make_ratio(int numerator, int denominator);
Atom make_ratio(int numerator, int denominator, bool simplify);
// Makes bignum from string of number
Atom make_bignum(std::string number);

// Checks if expr is proper list
bool listp(Atom expr);
// Returns length of list
int list_length(Atom list);
// Returns copy of list
Atom copy_list(Atom list);
// Gets index of list
Atom list_get(Atom list, int index);
// Sets index of list to value
void list_set(Atom list, int index, Atom value);
// Reverses list
void list_reverse(Atom* list);
// Returns if every element in list is of type
bool list_of_type_p(Atom list, Atom type);
// Returns if l1 and l2 have equal value
bool compare_lists(Atom l1, Atom l2);

// Converts atom string to std::string (as atom strings are really character lists)
std::string to_string(Atom str);

// Function call info to be pushed to stack
Atom make_frame(Atom parent, Atom environment, Atom pending);

// Table containing all symbols for keeping symbols unique
static Atom symbol_table = null;

// Returns length of bignum
int bignum_length(Atom bignum);
// Creates a bignum from an integer
Atom int_to_bignum(int integer);