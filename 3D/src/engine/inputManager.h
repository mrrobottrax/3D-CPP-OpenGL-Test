#pragma once

namespace inputManager {
	extern char keybindings[1024];

	void bindKey(int, char);
	void keyCallback(int, int);
}