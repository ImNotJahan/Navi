#include <string>
#include "../include/strings.h"

void remove_starting_chars(std::string& str, const char* charsToRemove)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (!find_char_p(str[i], charsToRemove))
        {
            str.erase(0, i);
            return;
        }
    }

    str = "";
}

int first_char(std::string str, const char* characters)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (find_char_p(str[i], characters)) return i;
    }

    return -1;
}

int first_char(std::string str, const char* characters, const char* escape)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (find_char_p(str[i], characters))
        {
            if (i == 0) return i; // if first char, escape char couldn't be before it

            for (int k = 0; k < strlen(escape); k++)
            {
                if (str[i - 1] == escape[k]) break;

                if (k + 1 == strlen(escape)) return i;
            }
        }
    }

    return -1;
}

bool has_char_p(std::string str, const char* characters)
{
    if (first_char(str, characters) == -1) return false;
    return true;
}

bool find_char_p(char character, const char* characters)
{
    for (int i = 0; i < strlen(characters); i++)
    {
        if (character == characters[i]) return true;
    }
    return false;
}

std::string to_uppercase(std::string str)
{
    std::string temp = "";

    for (int i = 0; i < str.length(); i++)
    {
        temp += toupper(str[i]);
    }

    return temp;
}