#pragma once

Atom add_bignums(Atom a, Atom b);
Atom subtract_bignums(Atom a, Atom b);
Atom multiply_bignums(Atom a, Atom b);
Atom divide_bignums(Atom a, Atom b);

// Returns if a < b
bool bignum_less(Atom a, Atom b);