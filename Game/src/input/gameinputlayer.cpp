#include <gamepch.h>
#include <input/gameinputlayer.h>

#include <input/inputsystem.h>
#include <imgui/imguiutil.h>

void GameKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
	inputSystem.KeyCallback(key, scancode, action, mods);

	if (key == GLFW_KEY_DELETE && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		showDemoWindow = !showDemoWindow;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
	}
}