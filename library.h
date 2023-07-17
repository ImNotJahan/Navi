#pragma once

enum LogLevel
{
	NONE,
	ERROR_ONLY,
	ALL
};

std::string read_file(std::string path);
void interpret_file(Atom environment, std::string path, LogLevel logLevel);