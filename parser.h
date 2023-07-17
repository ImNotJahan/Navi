#pragma once

// Handles ints, floats, chars, symbols, and null. Might split it up later
Error parse_simple(std::string start, std::string end, Atom* result);
// Handles lists
Error read_list(std::string start, std::string* end, Atom* result);
// Gets token from string
Error lexer(std::string str, std::string* start, std::string* end);

Error read_character(std::string str, Atom* result);
Error read_string(std::string start, std::string* end, Atom* result);