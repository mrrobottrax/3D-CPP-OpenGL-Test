#include <pch.h>
#include <input/malletinputlayer.h>

#include <inputmanager.h>
#include <main.h>

void MalletKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	inputManager::KeyCallback(key, action);

	if (key == GLFW_KEY_DELETE && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		showDemoWindow = !showDemoWindow;
	}
}