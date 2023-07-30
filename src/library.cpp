#include <fstream>
#include <vector>
#include <sstream>

#include "../include/main.h"
#include "../include/library.h"
#include "../include/evaluate.h"
#include "../include/data.h"

std::string read_file(std::string path)
{
	std::ifstream file(path);
	std::string contents;
	std::string temp;

	while (getline(file, temp)) contents += temp += "\n";

	file.close();

	return contents;
}

Error interpret_file(Atom environment, std::string path, LogLevel logLevel)
{
	return interpret_file(environment, path, logLevel, "*", "");
}

Error interpret_file(Atom environment, std::string path, LogLevel logLevel, std::string symbols_to_load)
{
	return interpret_file(environment, path, logLevel, symbols_to_load, "");
}

Error interpret_file(Atom environment, std::string path, LogLevel logLevel, std::string symbols_to_load, std::string prefix)
{
	if (path == "random_builtin") return load_random(environment);
	if (path == "time_builtin") return load_time(environment);
	if (path == "files_builtin") return load_files(environment);

	std::string text = read_file(path);
	
	if (text == "")
	{
		return Error{ Error::SYNTAX, "File " + path + " doesn't exist or is empty", "LOAD" };
	}

	std::vector<std::string> symbols;

	if (symbols_to_load != "*")
	{
		std::stringstream ss(symbols_to_load);
		std::string temp;
		while (ss >> temp)
		{
			for (auto& c : temp) c = (char)toupper(c);
			symbols.push_back(temp);
		}
	}

	Atom expr;
	Error err;

	// Read expressions in text
	while ((err = read_expr(text, &text, &expr)).type == Error::OK)
	{
		// Skips evaluating (set) expression if not included in load
		if (!symbols.empty())
		{
			if (listp(expr))
			{
				if (head(expr).type == Atom::SYMBOL && *head(expr).value.symbol == "SET")
				{
					if (head(tail(expr)).type == Atom::PAIR && head(head(tail(expr))).type == Atom::SYMBOL)
					{
						std::string name = *head(head(tail(expr))).value.symbol;

						auto itr = std::find(symbols.begin(), symbols.end(), name);
						if (itr == symbols.end())
						{
							continue;
						}
					}
				}
			}
		}

		Atom result;
		err = evaluate_expr(expr, environment, &result);

		if (err.type && err.type != Error::EMPTY && logLevel >= LogLevel::ERROR_ONLY)
		{			
			return err;
		}
		else if(logLevel == LogLevel::ALL)
		{
			say_expr(result);
			say_expr(make_character('\n'));
		}
	}

	if (err.type && err.details != "Missing token") return err; // missing token can be caused by file ending
	
	// Empty error so nothing is printed to REPL
	return Error{ Error::EMPTY };
}