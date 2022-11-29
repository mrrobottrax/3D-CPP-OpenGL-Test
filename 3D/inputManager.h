#pragma once

#include <glm/glm.hpp>

namespace inputManager {
	extern char keybindings[1024];

	void bindKey(int, char);
	void keyCallback(int, int);
}