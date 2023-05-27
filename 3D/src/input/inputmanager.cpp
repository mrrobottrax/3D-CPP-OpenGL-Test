#include <pch.h>
#include <input/inputmanager.h>

#include <gl/glutil.h>

InputManager::InputManager() : keys(), cursorDeltaX(0), cursorDeltaY(0), buttons()
{
	lastCursorPosX = 0;
	lastCursorPosY = 0;

	AddDefaultInputCommands();

	//TEMP
	BindKey('w', "+moveforward");
	BindKey('s', "+moveback");
	BindKey('a', "+moveleft");
	BindKey('d', "+moveright");

	BindKey(KEY_UpArrow, "+lookup");
	BindKey(KEY_DownArrow, "+lookdown");
	BindKey(KEY_LeftArrow, "+lookleft");
	BindKey(KEY_RightArrow, "+lookright");

	BindToggleKey('r', "r_draw");
	BindToggleKey('t', "timescale");
	BindKey('y', "advance");
}

InputManager::~InputManager()
{
	
}

void InputManager::BindKey(char key, const char* action)
{
	memset(keys[key].binding, NULL, MAX_BIND_LENGTH);
	strcpy_s(keys[key].binding, action);

	char* keyname = KeycodeToName(key);
	std::cout << "Bound " << keyname << " to " << action << "\n";
}

void InputManager::BindToggleKey(char key, const char* cvar)
{
	char* keyname = KeycodeToName(key);

	memset(keys[key].binding, NULL, MAX_BIND_LENGTH);
	strcpy_s(keys[key].binding, "toggle ");
	strcat_s(keys[key].binding, cvar);

	std::cout << "Bound " << keyname << " to toggle " << cvar << "\n";
}

int InputManager::KeyboardInputToKeycode(int key)
{
	if (IsPrintableASCII(key))
	{
		key = KeycodeToLowercase(key);
		return key;
	}

	switch (key)
	{
	case GLFW_KEY_UP:
		return KEY_UpArrow;
	case GLFW_KEY_DOWN:
		return KEY_DownArrow;
	case GLFW_KEY_LEFT:
		return KEY_LeftArrow;
	case GLFW_KEY_RIGHT:
		return KEY_RightArrow;
	default:
		return KEY_None;
	}
}

int InputManager::MouseInputToKeycode(int button)
{
	return 0;
}

char* InputManager::KeycodeToName(int keycode)
{
	if (keycode >= MAX_KEYS)
	{
		return "<KEYCODE OUT OF RANGE>";
	}

	static char name[MAX_KEYCODE_NAME_LENGTH];

	// Check for ASCII
	name[1] = NULL;
	name[0] = keycode;

	if (IsPrintableASCII(keycode))
	{
		return name;
	}

	// Check for name
	Keyname* pKn;
	for (pKn = keynames; pKn->name; pKn++)
	{
		if (keycode == pKn->keycode)
		{
			strcpy_s(name, pKn->name);
			return name;
		}
	}

	return "<UNKNOWN KEY>";
}

int InputManager::NameToKeycode(const char* name)
{
	if (IsPrintableASCII(name[0]))
	{
		return name[0];
	}
	
	Keyname* pKn;
	for (pKn = keynames; pKn->name; pKn++)
	{
		if (_strcmpi(name, pKn->name))
		{
			return pKn->keycode;
		}
	}
	
	return -1;
}

void InputManager::ButtonCallback(int keycode, int action)
{
	if (action == GLFW_PRESS)
	{
		keys[keycode].pressed = true;

		console.AddString(keys[keycode].binding);
		console.ParseInput(keycode);

		return;
	}

	if (action == GLFW_RELEASE)
	{
		keys[keycode].pressed = false;

		if (keys[keycode].binding != nullptr && keys[keycode].binding[0] == '+')
		{
			char releaseCommand[MAX_COMMAND_NAME_LENGTH];
			strcpy_s(releaseCommand, sizeof(releaseCommand), keys[keycode].binding);
			releaseCommand[sizeof(releaseCommand) - 1];

			releaseCommand[0] = '-';
			console.AddString(releaseCommand);
			console.ParseInput(keycode);
		}

		return;
	}
}

void InputManager::KeyCallback(int key, int scancode, int action, int mods)
{
	int keycode = KeyboardInputToKeycode(key);

	ButtonCallback(keycode, action);
}

void InputManager::MouseCallback(int button, int action)
{
	int keycode = MouseInputToKeycode(button);

	ButtonCallback(keycode, action);
}

const int loopMargin = 10;

void InputManager::UpdateCursorDelta(double xPos, double yPos)
{
	if (lastCursorPosX == 0 && lastCursorPosY == 0)
	{
		lastCursorPosX = xPos;
		lastCursorPosY = yPos;
		return;
	}

	cursorDeltaX = xPos - lastCursorPosX;
	cursorDeltaY = yPos - lastCursorPosY;

	lastCursorPosX = xPos;
	lastCursorPosY = yPos;

	// Loop cursor TODO: This should probably be in mallet code
	if (cursorLoop)
	{
		GLsizei windowSize[2];
		glfwGetWindowSize(pMainWindow, &windowSize[0], &windowSize[1]);

		double cursorPos[2];
		cursorPos[0] = xPos;
		cursorPos[1] = yPos;

		bool looped = false;

		for (int i = 0; i < 2; ++i)
		{
			while (cursorPos[i] < loopMargin)
			{
				cursorPos[i] += windowSize[i] - loopMargin * 2;
				looped = true;
			}

			while (cursorPos[i] > windowSize[i] - loopMargin)
			{
				cursorPos[i] -= windowSize[i] - loopMargin * 2;
				looped = true;
			}
		}

		glfwSetCursorPos(pMainWindow, cursorPos[0], cursorPos[1]);

		if (looped)
		{
			lastCursorPosX = cursorPos[0] - cursorDeltaX;
			lastCursorPosY = cursorPos[1] - cursorDeltaY;
		}
	}
}

void InputManager::GetCursorDelta(double* deltaX, double* deltaY)
{
	*deltaX = cursorDeltaX;
	*deltaY = cursorDeltaY;
}

void InputManager::ButtonDown(Button& button)
{
	if (!button.down[0] || button.down[0] == console.GetKey())
	{
		button.down[0] = console.GetKey();
		return;
	}

	if (!button.down[1] || button.down[1] == console.GetKey())
	{
		button.down[1] = console.GetKey();
		return;
	}
}

void InputManager::ButtonUp(Button& button)
{
	if (button.down[0] == console.GetKey())
	{
		button.down[0] = 0;
		return;
	}

	if (button.down[1] == console.GetKey())
	{
		button.down[1] = 0;
		return;
	}
}

bool InputManager::GetButtonDown(int buttonIndex)
{
	if (buttonIndex >= MAX_BUTTONS)
	{
		char number[3];
		number[0] = int(buttonIndex / 10) % 10;
		number[1] = buttonIndex % 10;

		console.Print("Requested value of out of range button ");
		console.Print(number);
		console.Print("\n");
		return false;
	}

	return buttons[buttonIndex].down[0] || buttons[buttonIndex].down[1];
}

void InputManager::SetCursorLoop(bool loop)
{
	cursorLoop = loop;
}

void InputManager::BindCommand()
{
	const char* keyName = console.GetNextArgs();
	const char* command = console.GetNextArgs();

	char key = NameToKeycode(keyName);

	BindKey(key, command);
}

void InputManager::BindToggleCommand()
{
	const char* keyName = console.GetNextArgs();
	const char* cvar = console.GetNextArgs();

	char key = NameToKeycode(keyName);

	BindToggleKey(key, cvar);
}

void InputManager::Cleanup()
{
	console.~Console();
}

void InputManager::AddDefaultInputCommands()
{
	console.AddCommand("bind", std::bind(&InputManager::BindCommand, this));
	console.AddCommand("bindtoggle", std::bind(&InputManager::BindToggleCommand, this));

	console.AddCommand("+moveforward", std::bind(&InputManager::MoveForwardDown, this));
	console.AddCommand("-moveforward", std::bind(&InputManager::MoveForwardUp, this));
	console.AddCommand("+moveback", std::bind(&InputManager::MoveBackDown, this));
	console.AddCommand("-moveback", std::bind(&InputManager::MoveBackUp, this));
	console.AddCommand("+moveleft", std::bind(&InputManager::MoveLeftDown, this));
	console.AddCommand("-moveleft", std::bind(&InputManager::MoveLeftUp, this));
	console.AddCommand("+moveright", std::bind(&InputManager::MoveRightDown, this));
	console.AddCommand("-moveright", std::bind(&InputManager::MoveRightUp, this));

	console.AddCommand("+lookup", std::bind(&InputManager::LookUpDown, this));
	console.AddCommand("-lookup", std::bind(&InputManager::LookUpUp, this));
	console.AddCommand("+lookdown", std::bind(&InputManager::LookDownDown, this));
	console.AddCommand("-lookdown", std::bind(&InputManager::LookDownUp, this));
	console.AddCommand("+lookleft", std::bind(&InputManager::LookLeftDown, this));
	console.AddCommand("-lookleft", std::bind(&InputManager::LookLeftUp, this));
	console.AddCommand("+lookright", std::bind(&InputManager::LookRightDown, this));
	console.AddCommand("-lookright", std::bind(&InputManager::LookRightUp, this));
}