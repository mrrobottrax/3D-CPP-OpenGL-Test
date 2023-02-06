#pragma once

#define MAX_KEYS 256

namespace InputManager {
	struct Key
	{
		bool pressed;

		char* binding;
	};

	extern Key keys[MAX_KEYS];

	void BindKey(int, char);
	void KeyCallback(int, int);
	void MouseCallback(int, int);

	void UpdateCursorDelta(double xPos, double yPos);
	void GetCursorDelta(double* deltaX, double* deltaY);
}