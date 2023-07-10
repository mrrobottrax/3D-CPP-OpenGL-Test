#include "pch.h"
#include "console.h"

Console::Console() : enabled(false), commands()
{
	memset(input, NULL, maxConsoleInputLength);

	AddCommand("cmd_test", TestCmd);
	AddCommand("cmd_toggle", ToggleConsoleCommand);
	AddCommand("echo", Echo);
	AddCommand("toggle", ToggleCvar);
}

Console::~Console()
{
	//DeleteCommands();
}

void Console::ToggleConsole()
{
	enabled = !enabled;
}

void Console::AddCommand(const char* name, function<void(Console&)> function)
{
	size_t length = strlen(name) + 1;
	char* heap_name = new char[length];
	strcpy_s(heap_name, length, name);

	commands.emplace(heap_name, function);
}

void Console::AddCvar(Cvar& cvar)
{
	if (CvarExists(cvar.name))
	{
		Print("Duplicate cvar being registered ");
		Print(cvar.name);
		Print("\n");
		return;
	}

	cvars.emplace(cvar.name, &cvar);
}

// WARNING: Inherits args from console
// Don't use on commands that use args
void Console::RunCommand(const char* name)
{
	if (!CmdExists(name))
	{
		if (!CvarExists(name))
		{
			Print("Could not find command ");
			Print(name);
			Print("\n");
			return;
		}

		SetCvar(name, GetArguments());
		return;
	}

	commands[name](*this);
}

void Console::RunCommand(const char* name, const char* args)
{
	strcpy_s(arguments, maxCommandArgsLength, args);
	argIndex = 0;

	RunCommand(name);
}

void Console::SetCvar(const char* name, const char* value)
{
	if (!CvarExists(name))
	{
		Print("Could not find cvar ");
		Print(name);
		Print("\n");
		return;
	}

	float fvalue = std::stof(value);

	cvars[name]->value = fvalue;
}

float Console::GetCvarValue(const char* name)
{
	return cvars[name]->value;
}

Cvar& Console::GetCvar(const char* name)
{
	return *cvars[name];
}

void Console::AddString(const char* string)
{
	if (string == nullptr)
		return;

	size_t len = strlen(string);
	strcpy_s(input + endIndex, maxConsoleInputLength - endIndex, string);

	endIndex += (int)len;
}

void Console::ParseInput(char key)
{
	if (input[0] == NULL)
		return;

	argIndex = 0;
	keycode = key;

	bool writingArgs = false;
	bool skippingSpaces = false;
	bool inQuotes = false;

	char command[maxCommandNameLength]{};

	int offset = 0;

	for (int i = 0; i < maxConsoleInputLength; i++)
	{
		if (input[i] == NULL || input[i] == ';' || input[i] == '\n')
		{
			if (writingArgs)
			{
				arguments[i - offset] = NULL;
				RunCommand(command);
			}
			else
			{
				command[i - offset] = NULL;
				RunCommand(command, "");
			}

			if (input[i] == NULL)
				break;

			writingArgs = false;
			offset = i + 1;
			skippingSpaces = true;
			continue;
		}

		if (input[i] == ' ')
		{
			if (skippingSpaces)
			{
				offset = i + 1;
				continue;
			}

			if (!writingArgs)
			{
				command[i - offset] = NULL;

				writingArgs = true;
				offset = i + 1;
				skippingSpaces = true;
				continue;
			}
		}

		skippingSpaces = false;

		if (input[i] == '\'' || input[i] == '"')
		{
			offset += 1;
			inQuotes = !inQuotes;
			continue;
		}

		if (!writingArgs)
		{
			command[i - offset] = input[i];
		}
		else
		{
			// Replace space with null
			if (input[i] == ' ' && !inQuotes)
			{
				arguments[i - offset] = NULL;
			}
			else
			{
				arguments[i - offset] = input[i];
			}
		}
	}

	endIndex = 0;
	input[0] = NULL;
}

void Console::DeleteCommands()
{
	for (auto const& command : commands)
	{
		if (command.first)
			delete command.first;
	}
}

bool Console::CmdExists(const char* name)
{
	return commands.find(name) != commands.end();
}

bool Console::CvarExists(const char* name)
{
	return cvars.find(name) != cvars.end();
}

const char* Console::GetNextArgs()
{
	int start = argIndex;

	for (int i = argIndex; i < maxCommandArgsLength; i++)
	{
		if (arguments[i] == NULL)
		{
			argIndex = i + 1;
			if (argIndex >= maxCommandArgsLength)
			{
				argIndex = maxCommandArgsLength;
			}
			break;
		}
	}

	return &arguments[start];
}

// COMMANDS

void Echo(Console& console)
{
	console.Print(console.GetArguments());
	console.Print("\n");
}

void TestCmd(Console& console)
{
	console.Println("TEST CMD!");
}

void ToggleConsoleCommand(Console& console)
{
	console.ToggleConsole();
	console.Print("TOGGLE CONSOLE");
}

void ToggleCvar(Console& console)
{
	const char* scvar = console.GetNextArgs();

	Cvar& cvar = console.GetCvar(scvar);

	if (cvar.value)
	{
		cvar.value = 0;
	}
	else
	{
		cvar.value = 1;
	}
}