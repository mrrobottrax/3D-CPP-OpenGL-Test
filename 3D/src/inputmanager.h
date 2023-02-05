#pragma once

namespace InputManager {
	extern char keybindings[1024];

	void BindKey(int, char);
	void KeyCallback(int, int);
	void MouseCallback(int, int);

	void UpdateCursorDelta(double xPos, double yPos);
	void GetCursorDelta(double* deltaX, double* deltaY);
}