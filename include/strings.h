#pragma once

// Removes charsToRemove from the start of str
void remove_starting_chars(std::string& str, const char* charsToRemove);

// Return if str contains character
bool has_char_p(std::string str, const char* characters);

// Return index of first instance of any char in characters in str, or -1 if none found
int first_char(std::string str, const char* characters);

// Return true if character equals any of characters
bool find_char_p(char character, const char* characters);

std::string to_uppercase(std::string str);