#include <pch.h>
#include <input/console.h>

//#include <managers.h>

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

void Console::AddCommand(const char* name, function<void(Console&)> function)
{
	size_t length = strlen(name);
	char* heap_name = new char[length + 1];
	strcpy(heap_name, name);

	commands.emplace(heap_name, function);

	/*Print("Added command: ");
	Println(heap_name);*/
}

void Console::RunCommand(const char* name)
{
	if (commands.find(name) == commands.end())
	{
		Print("Could not find command ");
		Print(name);
		Print("\n");
		return;
	}

	commands[name](*this);
}

void Console::RunCommand(const char* name, const char* args)
{
	strcpy(arguments, args);
	argIndex = 0;

	if (commands.find(name) == commands.end())
	{
		Print("Could not find command ");
		Print(name);
		Print("\n");
		return;
	}

	commands[name](*this);
}

void Console::AddString(const char* string)
{
	if (string == nullptr)
		return;

	size_t len = strlen(string);
	strcpy_s(input + endIndex, MAX_CONSOLE_INPUT_LENGTH - endIndex, string);

	endIndex += (int)len;
}

void Console::ParseInput(char key)
{
	if (input[0] == NULL)
		return;

	argIndex = 0;
	keycode = key;

	bool writingArgs = false;

	char command[MAX_COMMAND_NAME_LENGTH];

	int offset = 0;

	for (int i = 0; i < MAX_CONSOLE_INPUT_LENGTH; i++)
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
			offset = i;
			continue;
		}

		if (!writingArgs && input[i] == ' ')
		{
			command[i - offset] = NULL;

			writingArgs = true;
			offset = i;
			continue;
		}

		if (!writingArgs)
		{
			command[i - offset] = input[i];
		}
		else
		{
			// Replace space with null
			if (input[i] == ' ')
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

char* Console::GetNextArgs()
{
	int start = argIndex;

	for (int i = argIndex; i < MAX_COMMAND_ARGS_LENGTH; i++)
	{
		if (arguments[i] == NULL)
		{
			argIndex = i + 1;
			if (argIndex >= MAX_COMMAND_ARGS_LENGTH)
			{
				argIndex = MAX_COMMAND_ARGS_LENGTH;
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
}

void TestCmd(Console& console)
{
	const char* args = console.GetArguments();

	console.Println("TEST CMD!");
}

void ToggleConsoleCommand(Console& console)
{
	console.ToggleConsole();
	console.Print("TOGGLE CONSOLE");
}
