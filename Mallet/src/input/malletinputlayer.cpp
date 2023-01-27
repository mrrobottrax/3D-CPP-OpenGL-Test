#include <malletpch.h>
#include <input/malletinputlayer.h>

#include <inputmanager.h>
#include <imgui/imguiutils.h>

void MalletKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	inputManager::KeyCallback(key, action);

	if (key == GLFW_KEY_DELETE && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		showDemoWindow = !showDemoWindow;
	}
}