#include "command_parser.h"

#include <ctype.h>
#include <errno.h>

std::string ParseArg::err;

bool ParseArg::Long(long *ret, char **args)
{
	if(args == nullptr || *args == nullptr) {
		err = "No argument received";
		return false;
	}

	bool is_hex = false;
	char *id_end = strchr(*args, ' ');

	if(id_end == nullptr)
		id_end = *args + strlen(*args) - 1;

	for(int i = 0; i < id_end - *args; ++i) {
		char c = (*args)[i];

		if(isdigit(c) != 0)
			continue;

		/* Sign is only allowed as the first character. */
		if(c == '-' || c == '+') {
			if(i != 0) {
				err = "misplaced sign";
				return false;
			}
		}
		/*
		 * Only allow a hex specifier as the second digit, and when a sign is
		 * not specified.
		 */
		else if(c == 'x' || c == 'X') {
			if(isdigit(**args) == 0) {
				err = "invalid leading sign on hex specification";
				return false;
			}
			else if(i != 1 || is_hex == true) {
				err = "invalid_hex specification";
				return false;
			}
			is_hex = true;
		}
		/* At this point, anything else is invalid. */
		else {
			err = "invalid character";
			return false;
		}
	}

	errno = 0;
	long result = strtol(*args, NULL, is_hex ? 16 : 10);

	if(errno != 0) {
		err = "'";
		err.append(*args, id_end - *args);
		err.append("' out of range");
		return false;
	}

	if(ret != nullptr)
		*ret = result;

	*args = id_end + 1;

	if(*args != nullptr) {
		while(**args == ' ')
			++*args;
	}

	return true;
}

bool ParseArg::Double(double *ret, char **args)
{
	if(args == nullptr || *args == nullptr) {
		err = "No argument received";
		return false;
	}

	bool is_hex = false;
	char *id_end = strchr(*args, ' ');

	if(id_end == nullptr)
		id_end = *args + strlen(*args) - 1;

	for(int i = 0, found_dot = false; i < id_end - *args; ++i) {
		int c = (*args)[i];
		if(isdigit(c) != 0 || c == '\n')
			continue;

		/* Sign is only allowed as the first character */
		if(c == '-' || c == '+') {
			if(i != 0) {
				err = "misplaced sign";
				return false;
			}
		}
		/*
		 * Only allow a hex specifier at the second digit, and only with
		 * an integer value without a sign specified.
		 */
		else if(c == 'x' || c == 'X') {
			if(i != 1) {
				err = "invalid hex specification";
				return false;
			}
			else if(found_dot == true) {
				err = "floating point specifier in hex";
				return false;
			}
			else if(isdigit(**args) == 0) {
				err = "invalid leading sign on hex specification";
				return false;
			}
			is_hex = true;
		}
		/* Only allow one floating point specifier, and only in base 10. */
		else if(c == '.') {
			if(found_dot == true || is_hex == true) {
				err = is_hex ? "floating point specifier in hex" :
					"multiple_floating_point specifiers";
				return false;
			}
			found_dot = true;
		}
		/* At this point, anything else is invalid. */
		else {
			err = "invalid character";
			return false;
		}
	}

	errno = 0;
	double result = strtod(*args, NULL);

	if(errno != 0) {
		err = "'";
		err.append(*args, id_end - *args);
		err.append("' out of range");
		return 0;
	}

	if(ret != nullptr)
		*ret = result;
	*args = id_end + 1;

	while(**args == ' ')
		++*args;

	return true;
}

bool ParseArg::String(std::string *ret, char **args)
{
	if(args == nullptr || *args == nullptr) {
		err = "No argument received";
		return false;
	}

	char *id_end = strchr(*args, ' ');
	char quote_char; /* ' and " are both allowed to declare a string. */
	bool using_quotes = false;

	if(id_end == nullptr)
		id_end = *args + strlen(*args) - 1;

	/*
	 * If the string begins with a quote mark, we have to validate that there
	 * is a closing quote mark for the string, ignoring any escaped quote mark
	 * within.
	 */
	if((*args)[0] == '"' || (*args)[0] == '\'') {
		bool unescaped_quote = false;
		using_quotes = true;
		quote_char = (*args)[0];
		id_end = strchr(id_end + 1, quote_char);

		/*
		 * Iterate through the string trying to find out where the declaration
		 * ends.
		 */
		while(id_end != nullptr) {
			if(id_end[1] == ' ' || id_end[1] == '\0' || id_end[1] == '\n') {
				/*
				 * If we've hit the end of a word using an unescaped quote
				 * mark, consider it the end of the string declaration.
				 */
				if(*(id_end - 1) != '\\')
					break;
			}
			/*
			 * If an unescaped quote mark isn't at the end of a word, it's
			 * invalid.
			 */
			else if(*(id_end - 1) != '\\') {
				unescaped_quote = true;
				break;
			}

			id_end = strchr(id_end + 1, quote_char);
		}

		if(id_end == nullptr || unescaped_quote == true) {
			err = unescaped_quote ? "unescaped quote" : "missing end quote";
			return false;
		}
	}
	/*
	 * If the string is a word not wrapped in quote marks, validate all quote
	 * marks within the string are escaped.
	 */
	else {
		for(int i = 1; i < id_end - *args; ++i) {
			char c = (*args)[i];

			if((c == '\'' || c == '"') && (*args)[i - 1] != '\\') {
				err = "unescaped quote";
				return false;
			}
		}
	}

	if(using_quotes == false)
		++id_end;

	if(ret != nullptr) {
		/*
		 * If the string is wrapped in quote marks, omit them from the output.
		 */
		if(using_quotes)
			ret->append(*args + 1, id_end - *args - 1);
		else
			ret->append(*args, id_end - *args);
	}

	*args = id_end;

	if(using_quotes)
		++*args;

	while(**args == ' ')
		++*args;

	return true;
}

int ParseArg::Count(char *args)
{
	if(args == nullptr)
		return 0;

	int count = 0;

	while(*args != '\0') {
		if(args[0] == '\n')
			break;

		if(ParseArg::Long(nullptr, &args) ||
				ParseArg::Double(nullptr, &args) ||
				ParseArg::String(nullptr, &args))
			++count;
		else
			return -(++count);
	}

	return count;
}
