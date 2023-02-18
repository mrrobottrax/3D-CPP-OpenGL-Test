#pragma once
#include <input/keycodes.h>
#include <input/console.h>

class InputManager {
public:
	InputManager();
	~InputManager();

private:
	Key keys[MAX_KEYS];
	Button buttons[MAX_BUTTONS];

	double cursorDeltaX = 0;
	double cursorDeltaY = 0;

	double lastCursorPosX = 0;
	double lastCursorPosY = 0;

public:
	Console console;

public:
	void BindKey(char key, const char*);
	char InputToKeycode(int key, int mods);

#define MAX_KEYCODE_NAME_LENGTH 32
	void KeycodeToName(char keycode, char* string);
	char NameToKeycode(const char* name);
	void KeyCallback(int key, int scancode, int action, int mods);
	void MouseCallback(int, int);

	void UpdateCursorDelta(double xPos, double yPos);
	void GetCursorDelta(double* deltaX, double* deltaY);

	void ButtonDown(Button& button);
	void ButtonUp(Button& button);
	bool GetButtonDown(int);

	// Input Commands

	void AddDefaultInputCommands();
	void BindCommand();

	void MoveForwardDown()	{ ButtonDown(buttons[MoveForward]); };
	void MoveForwardUp()	{ ButtonUp  (buttons[MoveForward]); };
	void MoveBackDown()	{ ButtonDown(buttons[MoveBack]); };
	void MoveBackUp()	{ ButtonUp  (buttons[MoveBack]); };
	void MoveLeftDown()	{ ButtonDown(buttons[MoveLeft]); };
	void MoveLeftUp()	{ ButtonUp  (buttons[MoveLeft]); };
	void MoveRightDown()	{ ButtonDown(buttons[MoveRight]); };
	void MoveRightUp()		{ ButtonUp  (buttons[MoveRight]); };
	
	void LookUpDown()	{ ButtonDown(buttons[LookUp]); };
	void LookUpUp()		{ ButtonUp  (buttons[LookUp]); };
	void LookDownDown()	{ ButtonDown(buttons[LookDown]); };
	void LookDownUp()	{ ButtonUp  (buttons[LookDown]); };
	void LookLeftDown()	{ ButtonDown(buttons[LookLeft]); };
	void LookLeftUp()	{ ButtonUp  (buttons[LookLeft]); };
	void LookRightDown()	{ ButtonDown(buttons[LookRight]); };
	void LookRightUp()		{ ButtonUp  (buttons[LookRight]); };
};