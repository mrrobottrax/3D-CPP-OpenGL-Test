#pragma once

using namespace std;

#define MAX_COMMAND_NAME_LENGTH 32
#define MAX_COMMAND_ARGS_LENGTH 256
#define MAX_CONSOLE_INPUT_LENGTH 1024

class Console
{
public:
	Console();
	~Console();

private:
	bool enabled;

	char input[MAX_CONSOLE_INPUT_LENGTH];
	int endIndex = 0;

	map<const char*, function<void(const char*)>> commands;

public:
	void ToggleConsole();
	void AddCommand(const char* name, function<void(const char*)> function);
	void RunCommand(const char* name, const char* args);
	void AddString(const char* string);
	void ParseInput(int key);
};

void Echo(const char*);
void TestCmd(const char*);
void ToggleConsoleCommand(const char*);