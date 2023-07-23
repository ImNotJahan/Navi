#include "../include/main.h"
#include "../include/strings.h"
#include "../include/numbers.h"
#include "../include/parser.h"
#include "../include/data.h"

Error lexer(std::string str, std::string* start, std::string* end)
{
	const char* prefix = "`\'()\"";
	const char* delimiter = "() \t\n";
	const char* whitespace = " \t\n";

	//Remove whitespace
	remove_starting_chars(str, whitespace);

	// Occurs if string is only whitespace
	if (str == "") return Error{ Error::SYNTAX, "Missing token", "LEXER"};

	*start = str;

	// If has prefix, removes it
	if (find_char_p(str[0], prefix)) *end = str.substr(1);
	else if (str[0] == ',') *end = str.substr((str[1] == '@') ? 2 : 1); // unquote-splicing / unquote
	else if (str[0] == '~') // Single line comments 
	{
		int index = first_char(str, "\n");
		
		if (index == -1)
		{
			*end = "";
			*start = "null"; // this is here so it doesn't crash trying to handle nothing if there is only a comment in a program

			return Error{ Error::EMPTY };
		}

		str = str.substr(index);
		return lexer(str, start, end);
	}
	else if (str[0] == '|') // Multiline comments
	{
		str = str.substr(1);

		int index = first_char(str, "|");

		if (index == -1)
		{
			return Error{ Error::SYNTAX, "No comment end", "LEXER" };
		}
		
		str = str.substr(index + 1);

		return lexer(str, start, end);
	}
	// Otherwise the token ends when reaching a delimiter
	else
	{
		int index = first_char(str, delimiter);
		if (index == -1) *end = "";
		else
		{
			*end = str.substr(index);
		}
	}
	
	return NOERR;
}

Error read_expr(std::string input, std::string* end, Atom* result)
{
	std::string token;
	Error err;
	
	err = lexer(input, &token, end);
	if (err.type) return err;
	
	if (token[0] == '(') return read_list(*end, end, result);
	if (token[0] == ')') 
		return Error{ Error::SYNTAX, "Incorrect list syntax", "READ_EXPR"};
	
	if (token[0] == '\'')
	{
		*result = cons(sym("QUOTE"), cons(null, null));

		return read_expr(*end, end, &head(tail((*result))));
	}
	if (token[0] == ';')
	{
		*result = cons(sym("QUASIQUOTE"), cons(null, null));

		return read_expr(*end, end, &head(tail((*result))));
	}
	if (token[0] == ',')
	{
		*result = cons(sym(
			(token[1] == '@') ? "UNQUOTE-SPLICING" : "UNQUOTE"),
			cons(null, null));
		
		return read_expr(*end, end, &head(tail((*result))));
	}

	if (token[0] == '"') return read_string(*end, end, result);

	return parse_simple(token, *end, result);
}

Error parse_simple(std::string start, std::string end, Atom* result)
{
	std::string buf = start;
	buf = buf.substr(0, start.length() - end.length()); // remove end from string

	size_t endpos = 0;

	if (is_int(buf))
	{
		if (buf.length() > 9)
		{
			*result = make_bignum(buf);
		}
		else
		{
			long long_val = std::stol(buf, &endpos);

			result->type = Atom::INTEGER;
			result->value.integer = long_val;
		}
		return NOERR;
	}

	if (is_float(buf))
	{
		float fval = std::stof(buf, &endpos);

		result->type = Atom::FLOAT;
		result->value.float_ = fval;

		return NOERR;
	}

	if (buf[0] == '#') return read_character(buf, result);
	
	// Symbol bool or null
	
	buf = to_uppercase(buf);

	if (buf == "NULL") *result = null;
	else if (buf == "TRUE") *result = true_;
	else if (buf == "FALSE") *result = false_;
	else *result = sym(buf);

	return NOERR;
}

Error read_list(std::string start, std::string* end, Atom* result)
{
	Atom pair;

	*end = start;
	pair = *result = null;

	while (true)
	{
		std::string token;
		Atom item;
		Error err;

		err = lexer(*end, &token, end);
		if (err.type) return err;

		if (token[0] == ')') return NOERR;

		if (token[0] == '.')
		{
			if (nullp(pair)) return Error{ Error::SYNTAX, "Improper list", "READ_LIST"};

			err = read_expr(*end, end, &item);
			if (err.type) return err;

			tail(pair) = item;

			// Read the closing ')'
			err = lexer(*end, &token, end);

			if (!err.type && token[0] != ')') 
				err = Error{ Error::SYNTAX, "No closing parethesis", "READ_LIST"};

			return err;
		}

		err = read_expr(token, end, &item);
		if (err.type) return err;

		if (nullp(pair)) // Head
		{ 
			*result = cons(item, null);
			pair = *result;
		}
		else // Tail
		{
			tail(pair) = cons(item, null);
			pair = tail(pair);
		}
	}
}

Error read_character(std::string str, Atom* result)
{
	str = str.substr(1); // Remove # from string

	Error err = make_character(str, result);

	if (err.type) return err;
	return NOERR;
}

Error read_string(std::string start, std::string* end, Atom* result)
{
	int index = first_char(start, "\"", "\\");

	if (index == -1) return Error{ Error::SYNTAX, "String doesn't close", "READ_STRING" };

	std::string temp = start.substr(0, index);
	
	Error err = make_string(temp, result);
	if (err.type) return err;

	*end = start.substr(index + 1);

	return NOERR;
}