#pragma once

using namespace std;

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
	void ParseInput(const char* input, int key, bool down);
};

void Echo(const char*);
void TestCmd(const char*);
void ToggleConsoleCommand(const char*);