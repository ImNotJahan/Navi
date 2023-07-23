#pragma once

// Is string a float?
bool is_float(std::string str);
// Is string an int?
bool is_int(std::string str);
// Returns number of digits in integer
int int_length(int integer);

// Removes the digit at the start of a number (eg 1234 -> 234)
int remove_first_digit(int integer);
// Gets nth digit of a number
int nth_digit(int integer, int nth);