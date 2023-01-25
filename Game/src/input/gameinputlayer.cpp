#include <pch.h>
#include <input/gameinputlayer.h>

#include <inputmanager.h>
#include <main.h>

void GameKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	inputManager::KeyCallback(key, action);

	if (key == GLFW_KEY_DELETE && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		showDemoWindow = !showDemoWindow;
	}
}