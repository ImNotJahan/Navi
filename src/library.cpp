#include <fstream>
#include <iostream>
#include "../include/main.h"
#include "../include/library.h"
#include "../include/evaluate.h"

std::string read_file(std::string path)
{
	std::ifstream file(path);
	std::string contents;
	std::string temp;

	while (getline(file, temp)) contents += temp += "\n";

	file.close();

	return contents;
}

void interpret_file(Atom environment, std::string path, LogLevel logLevel)
{
	std::string text = read_file(path);
	
	if(logLevel == LogLevel::ALL) std::cout << "Reading " << path << std::endl;

	Atom expr;
	while (read_expr(text, &text, &expr).type == Error::OK)
	{
		Atom result;
		Error err = evaluate_expr(expr, environment, &result);

		if (err.type && logLevel >= LogLevel::ERROR_ONLY)
		{
			std::cout << "Error in expression: " << std::endl;
			print_expr(expr);
			std::cout << std::endl;
		}
		else if(logLevel == LogLevel::ALL)
		{
			print_expr(result);
			std::cout << std::endl;
		}
	}
}