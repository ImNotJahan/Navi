#pragma once

enum LogLevel
{
	NONE,
	ERROR_ONLY,
	ALL
};

std::string read_file(std::string path);
Error interpret_file(Atom environment, std::string path, LogLevel logLevel);
Error interpret_file(Atom environment, std::string path, LogLevel logLevel, std::string symbols_to_load);
Error interpret_file(Atom environment, std::string path, LogLevel logLevel, std::string symbols_to_load, std::string prefix);

Error load_random(Atom env);