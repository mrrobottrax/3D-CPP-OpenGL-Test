#include <pch.h>
#include <input/console.h>

Console::Console() : enabled(false), commands()
{
}

Console::~Console()
{
}

void Console::ToggleConsole()
{
	enabled = !enabled;
}

void Console::AddCommand(char* name, function<void(char*)> function)
{
	commands.emplace(name, function);
}

void Console::RunCommand(char* name, char* args)
{
	commands[name](args);
}
