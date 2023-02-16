#include <pch.h>
#include <input/inputmanager.h>

#include <gl/glutil.h>

InputManager::InputManager() : keys(), cursorDeltaX(0), cursorDeltaY(0), console()
{
	double cursorX, cursorY;
	glfwGetCursorPos(mainWindow, &cursorX, &cursorY);

	lastCursorPosX = cursorX;
	lastCursorPosY = cursorY;

	//TEMP
	BindKey('W', "+forward");
}

InputManager::~InputManager()
{
	for (int i = 0; i < MAX_KEYS; i++)
	{
		if (keys[i].binding != nullptr)
			delete[] keys[i].binding;
	}
}

void InputManager::BindKey(int key, const char* action)
{
	char* binding = new char[strlen(action) + 1];
	strcpy(binding, action);

	keys[key].binding = binding;

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
			strncpy(releaseCommand, keys[key].binding, sizeof(releaseCommand));
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

bool InputManager::GetActionDown(Action action)
{
	return false;
}