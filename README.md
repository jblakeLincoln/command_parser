# Command Parser

A basic way to parse commands and evaluate their arguments. Usage:

``` cpp
int func_print_hello(char *args) {
	printf("Hello world!\n");
	return 0;
}

CommandParser<> parser;
parser.Register("print_hello_msg", &func_print_hello);
parser.Parse("print_hello_msg");
```

Arguments can be evaluated with provided functions.

``` cpp
int func_add(char *args) {
	int args_received = ParseArgs::Count(args);
	int args_count = 0;
	long one, two;

	if(args_received != 2) {
		printf("Expected 2 arguments, received %d\n", args_received);
		return 0;
	}

	if((++args_count && ParseArgs::Long(&one, &args) &&
		++args_count && ParseArgs::Long(&two, &args)) == false)
	{
		printf("Unable to parse argument %d\n", args_count);
		return 0;
	}

	printf("%ld + %ld = %ld\n", one, two, one + two);
}
```
