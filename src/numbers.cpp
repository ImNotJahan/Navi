#include <string>
#include <sstream>
#include "../include/numbers.h"

bool is_float(std::string str)
{
    std::istringstream iss(str);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail();
}

bool is_int(std::string str)
{
    if (str[0] == '-' || str[0] == '+')
    {
        str.erase(0, 1);

        if (str.length() == 0) return false;
    }

    return str.find_first_not_of("0123456789") == std::string::npos;
}

int int_length(int integer)
{
    if (integer == 0) return 1;
    
    // Negative sign should be included
    if(integer < 0) return trunc(log10(abs(integer))) + 2;
    return trunc(log10(integer)) + 1;
}

int remove_first_digit(int integer)
{
    int new_number = 0;
    bool negative = false;

    if (integer < 0)
    {
        integer = std::abs(integer);
        negative = true;
    }

    int i = 0;
    while (integer > 9)
    {
        new_number += nth_digit(integer, 0) * std::pow(10, i);
        integer /= 10;
        i++;
    }

    if (negative) integer *= -1;

    return new_number;
}

int nth_digit(int integer, int nth)
{
    int temp = integer / std::pow(10, nth);
    return temp % 10;
}