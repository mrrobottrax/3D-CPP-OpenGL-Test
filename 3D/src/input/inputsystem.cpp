#include "pch.h"
#include "inputsystem.h"

#include "console.h"

#include <gl/glutil.h>

void InputSystem::Init()
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

void InputSystem::BindKey(char key, const char* action)
{
	memset(keys[key].binding, NULL, maxBindLength);
	strcpy_s(keys[key].binding, action);

	char* keyname = KeycodeToName(key);
	DEBUG_LOG("Bound " << keyname << " to " << action)
}

void InputSystem::BindToggleKey(char key, const char* cvar)
{
	char* keyname = KeycodeToName(key);

	memset(keys[key].binding, NULL, maxBindLength);
	strcpy_s(keys[key].binding, "toggle ");
	strcat_s(keys[key].binding, cvar);

	DEBUG_LOG("Bound " << keyname << " to toggle " << cvar)
}

KeyCode InputSystem::KeyboardInputToKeycode(int key)
{
	if (IsUppercase(key))
	{
		key = KeycodeToLowercase(key);
		return static_cast<KeyCode>(key);
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

KeyCode InputSystem::MouseInputToKeycode(int button)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		return KEY_MouseLeft;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		return KEY_MouseMiddle;
	case GLFW_MOUSE_BUTTON_RIGHT:
		return KEY_MouseRight;
	case GLFW_MOUSE_BUTTON_4:
		return KEY_Mouse4;
	case GLFW_MOUSE_BUTTON_5:
		return KEY_Mouse5;
	case GLFW_MOUSE_BUTTON_6:
		return KEY_Mouse6;
	case GLFW_MOUSE_BUTTON_7:
		return KEY_Mouse7;
	case GLFW_MOUSE_BUTTON_8:
		return KEY_Mouse8;
	default:
		return KEY_None;
	}
}

char* InputSystem::KeycodeToName(int keycode)
{
	if (keycode >= maxKeys)
	{
		return "<KEYCODE OUT OF RANGE>";
	}

	static char name[maxKeycodeNameLength];

	// Check for ASCII
	name[1] = NULL;
	name[0] = keycode;

	if (IsPrintableKeycode(keycode))
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

int InputSystem::NameToKeycode(const char* name)
{
	if (IsPrintableKeycode(name[0]))
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

void InputSystem::ButtonCallback(KeyCode keycode, int action)
{
	if (cursorFree)
	{
		return;
	}

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
			char releaseCommand[maxCommandNameLength];
			strcpy_s(releaseCommand, sizeof(releaseCommand), keys[keycode].binding);
			releaseCommand[sizeof(releaseCommand) - 1];

			releaseCommand[0] = '-';
			console.AddString(releaseCommand);
			console.ParseInput(keycode);
		}

		return;
	}
}

void InputSystem::KeyCallback(int key, int scancode, int action, int mods)
{
	KeyCode keycode = KeyboardInputToKeycode(key);

	ButtonCallback(keycode, action);
}

void InputSystem::MouseCallback(int button, int action)
{
	KeyCode keycode = MouseInputToKeycode(button);

	ButtonCallback(keycode, action);
}

void InputSystem::ScrollCallback(double xOffset, double yOffset)
{
	if (yOffset > 0)
	{
		ButtonCallback(KEY_ScrollUp, GLFW_PRESS);
		ButtonCallback(KEY_ScrollDown, GLFW_RELEASE);
	}
	else if (yOffset < 0)
	{
		ButtonCallback(KEY_ScrollDown, GLFW_PRESS);
		ButtonCallback(KEY_ScrollUp, GLFW_RELEASE);
	}
	else
	{
		ButtonCallback(KEY_ScrollDown, GLFW_RELEASE);
		ButtonCallback(KEY_ScrollUp, GLFW_RELEASE);
	}
}

void InputSystem::Update()
{
	double xPos, yPos;
	glfwGetCursorPos(pMainWindow, &xPos, &yPos);
	UpdateCursorDelta(xPos, yPos);
}

constexpr int loopMargin = 5;
void InputSystem::UpdateCursorDelta(double xPos, double yPos)
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

	// Loop cursor
	// TODO: This should probably be in mallet code
	if (cursorLoop)
	{
		int windowSize[2]{};
		glfwGetWindowSize(pMainWindow, &windowSize[0], &windowSize[1]);

		double cursorPos[2]{};
		cursorPos[0] = xPos;
		cursorPos[1] = yPos;

		bool looped = false;

		for (int i = 0; i < 2; ++i)
		{
			while (cursorPos[i] < loopMargin)
			{
				cursorPos[i] += static_cast<double>(windowSize[i]) - static_cast<double>(loopMargin) * 2.0;
				looped = true;
			}

			while (cursorPos[i] > (static_cast<double>(windowSize[i]) - static_cast<double>(loopMargin)))
			{
				cursorPos[i] -= static_cast<double>(windowSize[i]) - static_cast<double>(loopMargin) * 2.0;
				looped = true;
			}
		}

		glfwSetCursorPos(pMainWindow, cursorPos[0], cursorPos[1]);

		if (looped)
		{
			lastCursorPosX = cursorPos[0];
			lastCursorPosY = cursorPos[1];
		}
	}
}

void InputSystem::GetCursorDelta(double* deltaX, double* deltaY)
{
	if (cursorFree)
	{
		*deltaX = *deltaY = 0;
		return;
	}

	*deltaX = cursorDeltaX;
	*deltaY = cursorDeltaY;
}

void InputSystem::SetCursorFree(bool free)
{
	cursorFree = free;

	if (cursorFree)
		glfwSetInputMode(pMainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(pMainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputSystem::ToggleCursorFree()
{
	SetCursorFree(!cursorFree);
}

void InputSystem::ButtonDown(Button& button)
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

void InputSystem::ButtonUp(Button& button)
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

bool InputSystem::GetButtonDown(int buttonIndex)
{
	if (buttonIndex >= MAX_BUTTONS)
	{
		char number[3]{};
		number[0] = int(buttonIndex / 10) % 10;
		number[1] = buttonIndex % 10;

		console.Print("Requested value of out of range button ");
		console.Print(number);
		console.Print("\n");
		return false;
	}

	return buttons[buttonIndex].down[0] || buttons[buttonIndex].down[1];
}

void InputSystem::SetCursorLoop(bool loop)
{
	cursorLoop = loop;
}

void InputSystem::BindCommand()
{
	const char* keyName = console.GetNextArgs();
	const char* command = console.GetNextArgs();

	char key = NameToKeycode(keyName);

	BindKey(key, command);
}

void InputSystem::BindToggleCommand()
{
	const char* keyName = console.GetNextArgs();
	const char* cvar = console.GetNextArgs();

	char key = NameToKeycode(keyName);

	BindToggleKey(key, cvar);
}

void InputSystem::Cleanup()
{
	console.~Console();
}

void InputSystem::AddDefaultInputCommands()
{
	console.AddCommand("bind", std::bind(&InputSystem::BindCommand, this));
	console.AddCommand("bindtoggle", std::bind(&InputSystem::BindToggleCommand, this));

	console.AddCommand("+moveforward", std::bind(&InputSystem::MoveForwardDown, this));
	console.AddCommand("-moveforward", std::bind(&InputSystem::MoveForwardUp, this));
	console.AddCommand("+moveback", std::bind(&InputSystem::MoveBackDown, this));
	console.AddCommand("-moveback", std::bind(&InputSystem::MoveBackUp, this));
	console.AddCommand("+moveleft", std::bind(&InputSystem::MoveLeftDown, this));
	console.AddCommand("-moveleft", std::bind(&InputSystem::MoveLeftUp, this));
	console.AddCommand("+moveright", std::bind(&InputSystem::MoveRightDown, this));
	console.AddCommand("-moveright", std::bind(&InputSystem::MoveRightUp, this));

	console.AddCommand("+lookup", std::bind(&InputSystem::LookUpDown, this));
	console.AddCommand("-lookup", std::bind(&InputSystem::LookUpUp, this));
	console.AddCommand("+lookdown", std::bind(&InputSystem::LookDownDown, this));
	console.AddCommand("-lookdown", std::bind(&InputSystem::LookDownUp, this));
	console.AddCommand("+lookleft", std::bind(&InputSystem::LookLeftDown, this));
	console.AddCommand("-lookleft", std::bind(&InputSystem::LookLeftUp, this));
	console.AddCommand("+lookright", std::bind(&InputSystem::LookRightDown, this));
	console.AddCommand("-lookright", std::bind(&InputSystem::LookRightUp, this));
}