#include <string>
#include <sstream>
#include "numbers.h"

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