#include <pch.h>
#include <input/inputmanager.h>

#include <gl/glutil.h>

void MoveFowardDown(const char* args) {}
//void MoveFowardUp(const char* args) { ButtonUp(buttons[ButtonCode::MoveForward]); }

InputManager::InputManager() : keys(), cursorDeltaX(0), cursorDeltaY(0), console()
{
	double cursorX, cursorY;
	glfwGetCursorPos(mainWindow, &cursorX, &cursorY);

	lastCursorPosX = cursorX;
	lastCursorPosY = cursorY;

	//TEMP
	BindKey('W', "+forward");

	console.AddCommand("+moveforward", MoveForwardDown);
	console.AddCommand("-moveforward", MoveForwardUp);
	/*console.AddCommand("+moveback", MoveBackDown);
	console.AddCommand("-moveback", MoveBackUp);
	console.AddCommand("+moveleft", MoveLeftDown);
	console.AddCommand("-moveleft", MoveLeftUp);
	console.AddCommand("+moveright", MoveRightDown);
	console.AddCommand("-moveright", MoveRightUp);*/
}

InputManager::~InputManager()
{
	// Needed when bindings were pointers
	/*for (int i = 0; i < MAX_KEYS; i++)
	{
		if (keys[i].binding != nullptr)
			delete[] &keys[i].binding;
	}*/
}

void InputManager::BindKey(int key, const char* action)
{
	//size_t length = strlen(action);

	memset(keys[key].binding, NULL, MAX_CONSOLE_INPUT_LENGTH);
	strcpy(keys[key].binding, action);

	std::cout << "Bound " << (char)key << " to " << action << "\n";
}

void InputManager::KeyCallback(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
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
}

void InputManager::ButtonUp(Button& button)
{
}

bool InputManager::GetButtonDown(int buttonIndex)
{
	if (buttonIndex >= MAX_BUTTONS)
	{
		char number[3];
		number[0] = int(buttonIndex / 10) % 10;
		number[1] = buttonIndex % 10;

		Echo("Requested value of invalid button ");
		Echo(number);
		Echo("\n");
		return false;
	}

	return false;
}