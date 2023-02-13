#pragma once

using namespace std;

#define MAX_COMMAND_NAME_LENGTH 32
#define MAX_COMMAND_ARGS_LENGTH 256

class Console
{
public:
	Console();
	~Console();

private:
	bool enabled;

	map<const char*, function<void(const char*)>> commands;

public:
	void ToggleConsole();
	void AddCommand(const char* name, function<void(const char*)> function);
	void RunCommand(const char* name, const char* args);
	void ParseInput(const char* input, int key);
};

void Echo(const char*);
void TestCmd(const char*);
void ToggleConsoleCommand(const char*);