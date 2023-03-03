#pragma once

using namespace std;

#define MAX_COMMAND_NAME_LENGTH 32
#define MAX_COMMAND_ARGS_LENGTH 256
#define MAX_CONSOLE_INPUT_LENGTH 1024

struct cmp_str
{
	bool operator()(char const* a, char const* b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

class Console
{
public:
	Console();
	~Console();

private:
	bool enabled;

	char input[MAX_CONSOLE_INPUT_LENGTH];
	int endIndex = 0;

	char arguments[MAX_COMMAND_ARGS_LENGTH];
	int argIndex = 0;
	char keycode; // Key that called the most recent command, -1 for no key

	map<const char*, function<void(Console&)>, cmp_str> commands;

private:
	void RunCommand(const char* name);

public:
	void ToggleConsole();
	void AddCommand(const char* name, function<void(Console&)> function);
	void RunCommand(const char* name, const char* args);
	void AddString(const char* string);
	void ParseInput(char key);
	void DeleteCommands();
	inline char* GetArguments() { return arguments; };
	char* GetNextArgs();
	inline void SetArguments(const char* args) { strcpy_s(arguments, MAX_COMMAND_ARGS_LENGTH, args); }
	inline char GetKey() { return keycode; };
	inline void Print(const char* message) { std::cout << message; }
	inline void Println(const char* message) { std::cout << message << "\n"; }
}; inline Console console;

void Echo(Console& console);
void TestCmd(Console& console);
void ToggleConsoleCommand(Console& console);