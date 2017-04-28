#include "command_parser.h"

#include <string>
#include <vector>

/**
 * Delegate argument counting to a different function to keep a consistent
 * error message.
 */
int validate_arg_count(int expected, char *args) {
	int args_received = ParseArg::Count(args);

	if(args_received == -1) {
		printf("Unable to parse arguments: %s\n",
				ParseArg::GetLastError().c_str());
		return -1;
	}
	else if(args_received != expected) {
		printf("Expected %d arguments, received %d\n", expected, args_received);
		return -1;
	}

	return 0;
}

int func_help(char *args) {
	printf(
			"Available functions: \n"
			" * print_long(l)\n"
			" * print_double(d)\n"
			" * print_string(s)\n"
			" * print_vec3(d, d, d)\n"
			" * run_tests\n"
		  );
	return 0;
}

int func_print_string(char *args) {
	if(validate_arg_count(1, args) < 0)
		return 0;

	std::string s;

	if(ParseArg::String(&s, &args) == false) {
		printf("Error: %s\n", ParseArg::GetLastError().c_str());
		return 0;
	}

	printf("%s\n", s.c_str());
	return 0;
}

int func_print_double(char *args) {
	if(validate_arg_count(1, args) < 0)
		return 0;

	double d;

	if(ParseArg::Double(&d, &args) == false) {
		printf("Error: %s\n", ParseArg::GetLastError().c_str());
		return 0;
	}

	printf("%f\n", d);
	return 0;
}

int func_print_long(char *args) {
	if(validate_arg_count(1, args) < 0)
		return 0;

	long l;

	if(ParseArg::Long(&l, &args) == false) {
		printf("Error: %s\n", ParseArg::GetLastError().c_str());
		return 0;
	}

	printf("%li\n", l);
	return 0;
}

int func_print_vec3(char *args) {
	if(validate_arg_count(3, args) < 0)
		return 0;

	double x, y, z;
	int arg_count = 0;

	if((++arg_count && ParseArg::Double(&x, &args) &&
		++arg_count && ParseArg::Double(&y, &args) &&
		++arg_count && ParseArg::Double(&z, &args)) == false)
	{
		printf("Error parsing arg%d: %s\n", arg_count,
				ParseArg::GetLastError().c_str());
		return 0;
	}

	printf("{ %f, %f, %f }\n", x, y, z);
	return 0;
}
