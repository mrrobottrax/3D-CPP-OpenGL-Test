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

	void ButtonCallback(int, int);

public:
	Console console;

public:
	void BindKey(char key, const char*);
	int KeyboardInputToKeycode(int key);
	int MouseInputToKeycode(int button);

	char* KeycodeToName(int keycode);
	int NameToKeycode(const char* name);
	void KeyCallback(int key, int scancode, int action, int mods);
	void MouseCallback(int, int);

	void UpdateCursorDelta(double xPos, double yPos);
	void GetCursorDelta(double* deltaX, double* deltaY);

	void ButtonDown(Button& button);
	void ButtonUp(Button& button);
	bool GetButtonDown(int);

	// ~~~~~~~~~~~Input Commands~~~~~~~~~~~~~~~~~~~~~~~~~

	void BindCommand();
	void AddDefaultInputCommands();

	void MoveForwardDown() { ButtonDown(buttons[IN_MoveForward]); };
	void MoveForwardUp() { ButtonUp(buttons[IN_MoveForward]); };
	void MoveBackDown() { ButtonDown(buttons[IN_MoveBack]); };
	void MoveBackUp() { ButtonUp(buttons[IN_MoveBack]); };
	void MoveLeftDown() { ButtonDown(buttons[IN_MoveLeft]); };
	void MoveLeftUp() { ButtonUp(buttons[IN_MoveLeft]); };
	void MoveRightDown() { ButtonDown(buttons[IN_MoveRight]); };
	void MoveRightUp() { ButtonUp(buttons[IN_MoveRight]); };

	void LookUpDown() { ButtonDown(buttons[IN_LookUp]); };
	void LookUpUp() { ButtonUp(buttons[IN_LookUp]); };
	void LookDownDown() { ButtonDown(buttons[IN_LookDown]); };
	void LookDownUp() { ButtonUp(buttons[IN_LookDown]); };
	void LookLeftDown() { ButtonDown(buttons[IN_LookLeft]); };
	void LookLeftUp() { ButtonUp(buttons[IN_LookLeft]); };
	void LookRightDown() { ButtonDown(buttons[IN_LookRight]); };
	void LookRightUp() { ButtonUp(buttons[IN_LookRight]); };
};