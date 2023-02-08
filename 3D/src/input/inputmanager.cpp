#include <pch.h>
#include <input/inputmanager.h>

#include <gl/glutil.h>

InputManager::InputManager() : keys(), cursorDeltaX(0), cursorDeltaY(0), console()
{
	double cursorX, cursorY;
	glfwGetCursorPos(mainWindow, &cursorX, &cursorY);

	lastCursorPosX = cursorX;
	lastCursorPosY = cursorY;
}

InputManager::~InputManager()
{
}

void InputManager::BindKey(int key, char action)
{

}

void InputManager::KeyCallback(int key, int scancode, int action, int mods)
{
	std::cout << (char)key << ", " << key << "\n";

	if (action == GLFW_PRESS)
	{
		keys[key].pressed = true;

		console.ParseInput(keys[key].binding, key, true);
		return;
	}

	if (action == GLFW_RELEASE)
	{
		keys[key].pressed = false;

		console.ParseInput(keys[key].binding, key, false);
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