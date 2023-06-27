#pragma once
#include "keycodes.h"

#include <systems/system.h>

class InputSystem : public System
{
public:
	InputSystem() : keys(), cursorDeltaX(0), cursorDeltaY(0), buttons()
	{};

	~InputSystem()
	{};

private:
	Key keys[MAX_KEYS];
	Button buttons[MAX_BUTTONS];

	double cursorDeltaX = 0;
	double cursorDeltaY = 0;

	double lastCursorPosX = 0;
	double lastCursorPosY = 0;

	bool cursorLoop = false;
	bool cursorFree = false;

	void ButtonCallback(int, int);

public:
	void Update() override;
	void Init() override;

public:
	void BindKey(char key, const char*);
	void BindToggleKey(char key, const char*);
	int KeyboardInputToKeycode(int key);
	int MouseInputToKeycode(int button);

	char* KeycodeToName(int keycode);
	int NameToKeycode(const char* name);
	void KeyCallback(int key, int scancode, int action, int mods);
	void MouseCallback(int, int);

	void UpdateCursorDelta(double xPos, double yPos);
	void GetCursorDelta(double* deltaX, double* deltaY);
	void ToggleCursorFree();
	void SetCursorFree(bool);

	void ButtonDown(Button& button);
	void ButtonUp(Button& button);
	bool GetButtonDown(int);

	void SetCursorLoop(bool);

	void Cleanup();

	// ~~~~~~~~~~~Input Commands~~~~~~~~~~~~~~~~~~~~~~~~~

	void BindCommand();
	void BindToggleCommand();
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
}; inline InputSystem inputSystem;