#include <gamepch.h>
#include <input/gameinputlayer.h>

#include <input/inputmanager.h>
#include <imgui/imguiutil.h>

void GameKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	inputManager.KeyCallback(key, scancode, action, mods);

	if (key == GLFW_KEY_DELETE && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		showDemoWindow = !showDemoWindow;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}