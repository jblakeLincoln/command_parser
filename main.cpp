#if 0
time clang++ main.cpp                                                          \
	-Wall -Wswitch-default -Wcast-align -Wpointer-arith                        \
	-Winline -Wundef -Wcast-qual -Wshadow -Wwrite-strings                      \
	-Wunreachable-code -fno-common -Wunused-function                           \
	-Wuninitialized -Wtype-limits -Wsign-compare -Wmissing-braces              \
	-Wparentheses \
	--std=c++11 -pedantic -o a.out && ./a.out

exit
#endif

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "command_parser.cpp"
#include "example_functions.cpp"

/**
 * Here we set up a command parser, and register some callbacks to it, defined
 * in example_functions.cpp. We also register func_test, which uses the parser
 * declared in this file for test inputs.
 */

void do_test(std::string cmd, const std::vector<std::string> &vals);
int func_test(char *args);

CommandParser<> parser;

/**
 * Continually receive input from the terminal and parse the provided commands.
 * If the command is "exit", break out.
 */
void receive_input()
{
	while(true) {
		char buf[1024];
		fputs("$ ", stdout);
		fgets(buf, sizeof(buf), stdin);
		if(parser.ParseCommand(buf) == INT_MIN) {
			if(parser.GetLastCommand() == "exit")
				return;
			printf("Command '%s' not found\n", parser.GetLastCommand());
		}
	}
}

int main ()
{
	parser.Register("help", &func_help);
	parser.Register("print_double", &func_print_double);
	parser.Register("print_long", &func_print_long);
	parser.Register("print_string", &func_print_string);
	parser.Register("print_vec3", &func_print_vec3);
	parser.Register("test", &func_test);

	receive_input();

	return 0;
}

void do_test(std::string cmd, const std::vector<std::string> &vals)
{
	std::string full_cmd = "";
	for(size_t i = 0; i < vals.size(); ++i) {
		full_cmd = cmd + " " + vals[i];
		printf("%s\n", full_cmd.c_str());
		parser.ParseCommand(full_cmd.c_str());
	}
}

int func_test(char *args)
{
	if(validate_arg_count(0, args) < 0)
		return 0;

	std::vector<std::string> valid_doubles = {
		"+816", "-816", "0x32", "+0.5", "-0.5"
	};
	std::vector<std::string> invalid_doubles = {
		"8+16", "++816", "+0x32", "+0.5.f", "0.5.5", "0x32.5"
	};

	std::vector<std::string> valid_longs = {
		"816", "-816", "0x32"
	};
	std::vector<std::string> invalid_longs = {
		"8+16", "++816", "+0x32", "+0.5", "0.2"
	};

	std::vector<std::string> valid_strings = {
		"Hello",
		"Hel\"lo",
		"\"Hello world\"",
		"\"He\\\"llo\\\" 'something' w\\\"o\\\"\\\"l\\\"d\"",
		"Hello",
		"Hel\"lo",
		"'Hello world'",
		"'He\\'llo\\' \\'something\\' w\\'o\\'\\'l\\'d'"
	};
	std::vector<std::string> invalid_strings = {
		"Hello world", /* Split into */
		"Hel\"lo", /* Unescaped quote */
		"\"He\\\"llo\\\" w\"o\\\"\\\"l\\\"d\"", /* Unescaped quote after 'w' */
	};

	printf("\n** Valid inputs: \n");
	do_test("print_double", valid_doubles);
	do_test("print_long", valid_longs);
	do_test("print_string", valid_strings);
	printf("*****\n\n");
	printf("** Invalid inputs:\n");
	do_test("print_double", invalid_doubles);
	do_test("print_long", invalid_longs);
	do_test("print_string", invalid_strings);

	return 0;
}

