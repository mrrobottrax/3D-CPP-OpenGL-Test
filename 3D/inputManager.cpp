#include <inputManager.h>

#include <iostream>
#include <gl.h>

namespace inputManager {
	char keybindings[256];

	void bindKey(int key, char action)
	{
		
	}

	void keyCallback(int key, int action)
	{
		if (action == GLFW_PRESS)
		{
			keybindings[key] = 1;
			return;
		}

		if (action == GLFW_RELEASE)
		{
			keybindings[key] = 0;
			return;
		}
	}
}