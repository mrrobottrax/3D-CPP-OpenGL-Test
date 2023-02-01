#pragma once

namespace InputManager {
	extern char keybindings[1024];

	void BindKey(int, char);
	void KeyCallback(int, int);
}