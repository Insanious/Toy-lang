#ifndef GLOBALS_H
#define GLOBALS_H

extern int lineNumber;
extern bool debug_lex;
extern bool debug_grammar;
extern bool log_output;
extern bool debug_executions;
extern bool debug_binOps;

#include <iostream>
#include <string>

// static void fatal(std::string msg)
// {
// 	std::cout << "FATAL:\t\t" << msg << '\n';
// 	exit(0);
// }
//
// static void log(std::string msg)
// {
// 	if (log_output)
// 		std::cout << "LOG:\t\t" << msg << '\n';
// }


#endif
