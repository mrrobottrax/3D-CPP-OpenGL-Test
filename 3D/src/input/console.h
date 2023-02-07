#pragma once

using namespace std;

class Console
{
public:
	Console();
	~Console();

private:
	bool enabled;

	map<char*, function<void(char*)>> commands;

public:
	void ToggleConsole();
	void AddCommand(char* name, function<void(char*)> function);
	void RunCommand(char* name, char* args);
};