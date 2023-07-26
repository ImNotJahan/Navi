#pragma once

Atom add_bignums(Atom a, Atom b);
Atom subtract_bignums(Atom a, Atom b);
Atom multiply_bignums(Atom a, Atom b);
Atom divide_bignums(Atom a, Atom b);

// Faster method of multiplication
// https://en.wikipedia.org/wiki/Karatsuba_algorithm
Atom karatsuba_bignums(Atom a, Atom b);

// Returns if a < b
bool bignum_less(Atom a, Atom b);

// Multiplies bignum by 10
Atom shift_bignum(Atom bignum);
// Multiplies bignum by 10^times
Atom shift_bignum(Atom bignum, int times);

// Splits bignum at middle into low and high. Middle is actually numbers from right
void split_bignum(Atom bignum, int middle, Atom* low, Atom* high);