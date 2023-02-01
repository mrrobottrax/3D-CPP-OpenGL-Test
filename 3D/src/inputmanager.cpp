#include <pch.h>

#include <inputManager.h>

namespace InputManager {
	char keybindings[1024];

	void BindKey(int key, char action)
	{
		
	}

	void KeyCallback(int key, int action)
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