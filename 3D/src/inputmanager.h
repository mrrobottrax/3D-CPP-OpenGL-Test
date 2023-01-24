#pragma once

namespace inputManager {
	extern char keybindings[1024];

	void BindKey(int, char);
	void KeyCallback(int, int);
}