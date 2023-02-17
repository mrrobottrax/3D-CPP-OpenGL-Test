#include <pch.h>
#include <input/inputmanager.h>

#include <gl/glutil.h>

InputManager::InputManager() : keys(), cursorDeltaX(0), cursorDeltaY(0), console(), buttons()
{
	double cursorX, cursorY;
	glfwGetCursorPos(mainWindow, &cursorX, &cursorY);

	lastCursorPosX = cursorX;
	lastCursorPosY = cursorY;

	AddDefaultInputCommands();

	//TEMP
	BindKey('W', "+moveforward");
	BindKey('S', "+moveback");
	BindKey('A', "+moveleft");
	BindKey('D', "+moveright");

	BindKey(UpArrow, "+lookup");
	BindKey(DownArrow, "+lookdown");
	BindKey(LeftArrow, "+lookleft");
	BindKey(RightArrow, "+lookright");
}

InputManager::~InputManager()
{
	
}

void InputManager::BindKey(char key, const char* action)
{
	memset(keys[key].binding, NULL, MAX_CONSOLE_INPUT_LENGTH);
	strcpy(keys[key].binding, action);

	char keyname[MAX_KEYCODE_NAME_LENGTH];
	KeycodeToName(key, keyname);
	std::cout << "Bound " << keyname << " to " << action << "\n";
}

char InputManager::InputToKeycode(int key, int mods)
{
	return 0;
}

void InputManager::KeycodeToName(char keycode, char* string)
{
	string[0] = NULL;

	// ASCII Uppercase
	if (keycode >= 41 && keycode <= 90)
	{
		string[0] = keycode;
		string[1] = NULL;
	}
	
}

void InputManager::KeyCallback(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key >= MAX_KEYS)
			return;

		keys[key].pressed = true;

		console.AddString(keys[key].binding);
		console.ParseInput(key);

		return;
	}

	if (action == GLFW_RELEASE)
	{
		keys[key].pressed = false;

		if (keys[key].binding != nullptr && keys[key].binding[0] == '+')
		{
			char releaseCommand[MAX_COMMAND_NAME_LENGTH];
			strcpy_s(releaseCommand, sizeof(releaseCommand), keys[key].binding);
			releaseCommand[sizeof(releaseCommand) - 1];

			releaseCommand[0] = '-';
			console.AddString(releaseCommand);
			console.ParseInput(key);
		}

		return;
	}
}

void InputManager::MouseCallback(int button, int action)
{
	if (action == GLFW_PRESS)
	{

		return;
	}

	if (action == GLFW_RELEASE)
	{

		return;
	}
}

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

		console.Print("Requested value of invalid button ");
		console.Print(number);
		console.Print("\n");
		return false;
	}

	return buttons[buttonIndex].down[0] || buttons[buttonIndex].down[1];
}

void InputManager::AddDefaultInputCommands()
{
	console.AddCommand("+moveforward", std::bind(&InputManager::MoveForwardDown, this));
	console.AddCommand("-moveforward", std::bind(&InputManager::MoveForwardUp, this));
	console.AddCommand("+moveback", std::bind(&InputManager::MoveBackDown, this));
	console.AddCommand("-moveback", std::bind(&InputManager::MoveBackUp, this));
	console.AddCommand("+moveleft", std::bind(&InputManager::MoveLeftDown, this));
	console.AddCommand("-moveleft", std::bind(&InputManager::MoveLeftUp, this));
	console.AddCommand("+moveright", std::bind(&InputManager::MoveRightDown, this));
	console.AddCommand("-moveright", std::bind(&InputManager::MoveRightUp, this));
}
