#include <pch.h>

#include <inputManager.h>

namespace InputManager {
	char keybindings[1024];

	double cursorDeltaX = 0;
	double cursorDeltaY = 0;

	double lastCursorPosX = 0;
	double lastCursorPosY = 0;

	void BindKey(int key, char action)
	{
		
	}

	void KeyCallback(int key, int action)
	{
		if (action == GLFW_PRESS)
		{
			keybindings[key] = 1;
			return;
		}

		if (action == GLFW_RELEASE)
		{
			keybindings[key] = 0;
			return;
		}
	}

	void UpdateCursorDelta(double xPos, double yPos)
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

	void GetCursorDelta(double* deltaX, double* deltaY)
	{
		*deltaX = cursorDeltaX;
		*deltaY = cursorDeltaY;
	}
}