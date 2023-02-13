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
	map<const char*, function<void(const char*)>>::iterator it = commands.find(name);

	if (it == commands.end())
	{
		string commandName = string(name);
		string message = "Could not find command " + commandName;
		Echo(message.c_str());
		return;
	}

	commands[name](args);
}

void Console::ParseInput(const char* input, int key)
{
	if (input == nullptr)
		return;
	if (input[0] == NULL)
		return;

	bool writingArgs = false;

	char args[MAX_COMMAND_ARGS_LENGTH];
	char command[MAX_COMMAND_NAME_LENGTH];

	int offset = 0;

	for (int i = 0; i < 1024; i++)
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
