#include <pch.h>
#include <input/console.h>

#include <managers.h>

Console::Console() : enabled(false), commands()
{
	memset(input, NULL, MAX_CONSOLE_INPUT_LENGTH);

	AddCommand("cmd_test", TestCmd);
	AddCommand("cmd_toggle", ToggleConsoleCommand);
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

void Console::AddCommand(const char* name, function<void()> function)
{
	size_t length = strlen(name);
	char* heap_name = new char[length + 1];
	strcpy(heap_name, name);

	commands.emplace(heap_name, function);
}

void Console::RunCommand(const char* name, const char* args)
{
	strcpy(arguments, args);

	map<const char*, function<void()>>::iterator it = commands.find(name);

	if (it == commands.end())
	{
		Echo("Could not find command ");
		Echo(name);
		Echo("\n");
		return;
	}

	commands[name]();
}

void Console::AddString(const char* string)
{
	if (string == nullptr)
		return;

	size_t len = strlen(string);
	strcpy_s(input + endIndex, MAX_CONSOLE_INPUT_LENGTH - endIndex, string);

	endIndex += (int)len;
}

void Console::ParseInput(int key)
{
	if (input[0] == NULL)
		return;

	bool writingArgs = false;

	char args[MAX_COMMAND_ARGS_LENGTH];
	char command[MAX_COMMAND_NAME_LENGTH];

	int offset = 0;

	for (int i = 0; i < MAX_CONSOLE_INPUT_LENGTH; i++)
	{
		if (input[i] == NULL || input[i] == ';' || input[i] == '\n')
		{
			if (writingArgs)
			{
				args[i - offset] = NULL;
				RunCommand(command, args);
			}
			else
			{
				command[i - offset] = NULL;
				RunCommand(command, "");
			}

			if (input[i] == NULL)
				break;

			writingArgs = false;
			offset = i;
			continue;
		}

		if (input[i] == ' ')
		{
			command[i - offset] = NULL;

			writingArgs = true;
			offset = i;
			continue;
		}

		if (!writingArgs)
			command[i - offset] = input[i];
		else
			args[i - offset] = input[i];
	}

	endIndex = 0;
	input[0] = NULL;
}

// COMMANDS
void Echo(const char* args)
{
	std::cout << args;
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
