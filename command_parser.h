#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdlib.h>
#include <string.h>
#include <string>
#include <unordered_map>

template<typename... CallbackArgs>
struct CommandParser {
private:
	std::string last_command;
	std::unordered_map<std::string, int(*)(CallbackArgs..., char*)> callbacks;

public:
	const std::string& GetLastCommand() const {
		return last_command;
	}

	void Register(const char *cmd, int(*func)(CallbackArgs..., char*)) {
		callbacks[cmd] = func;
	}

	/**
	 * Split a provided console line into the command and arguments, and send
	 * them forward.
	 */
	int ParseCommand(const char *line, CallbackArgs ...other_args) {
		const char *args_temp = strchr(line, ' ');
		char *args = nullptr;
		char *cmd = nullptr;

		if(args_temp != NULL) {
			args = static_cast<char*>(strdup(args_temp + 1));
			cmd = static_cast<char*>(malloc(args_temp - line));
			strncpy(cmd, line, args_temp - line);
			cmd[args_temp - line] = '\0';
		}
		else {
			cmd = static_cast<char*>(malloc(strlen(line)));
			strncpy(cmd, line, strlen(line) - 1);
			cmd[strlen(line) - 1] = '\0';
		}

		last_command = cmd;

		if(callbacks.count(cmd) == 0) {
			printf("uhoh\n");
			return INT_MIN;
		}

		char *args_ptr = args;

		if(args_ptr != nullptr) {
			while(*args_ptr == ' ')
				++args_ptr;
		}

		int ret = callbacks[cmd](other_args..., args_ptr);

		free(cmd);
		free(args);

		return ret;
	}
};

struct ParseArg {
private:
	static std::string err;

public:
	static const std::string& GetLastError() {
		return err;
	}

	static bool Long(long *ret, char **args);
	static bool Double(double *ret, char **args);
	static bool String(std::string *ret, char **args);
	static int Count(char *args);
};

#endif
