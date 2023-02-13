#pragma once
#include <input/keycodes.h>
#include <input/console.h>

#define MAX_KEYS 256

struct Key
{
	bool pressed;

	char* binding;
};

class InputManager {
public:
	InputManager();
	~InputManager();

private:
	Key keys[MAX_KEYS];

	double cursorDeltaX = 0;
	double cursorDeltaY = 0;

	double lastCursorPosX = 0;
	double lastCursorPosY = 0;

public:
	Console console;

public:
	void BindKey(int, const char*);
	void KeyCallback(int key, int scancode, int action, int mods);
	void MouseCallback(int, int);

	void UpdateCursorDelta(double xPos, double yPos);
	void GetCursorDelta(double* deltaX, double* deltaY);

	bool GetActionDown(Action);
};