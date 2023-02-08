#include <pch.h>
#include <input/console.h>

#include <managers.h>

Console::Console() : enabled(false), commands()
{
	AddCommand("cmd_test", TestCmd);
	AddCommand("console_toggle", ToggleConsoleCommand);
	AddCommand("echo", Echo);
}

Console::~Console()
{
	for (auto const& command : commands)
	{
		delete command.first;
	}
}

void Console::ToggleConsole()
{
	enabled = !enabled;
}

void Console::AddCommand(const char* name, function<void(const char*)> function)
{
	size_t length = strlen(name);
	char* heap_name = new char[length + 1];
	strcpy(heap_name, name);

	commands.emplace(heap_name, function);
}

void Console::RunCommand(const char* name, const char* args)
{
	commands[name](args);
}

void Console::ParseInput(const char* input, int key, bool down)
{
}

// COMMANDS

void Echo(const char* args)
{
	std::cout << args << "\n";
}

void TestCmd(const char* args)
{
	std::cout << "TEST: " << args << "\n";

	Echo(args);
}

void ToggleConsoleCommand(const char* args)
{
	inputManager->console.ToggleConsole();
	Echo("TOGGLE CONSOLE");
}
