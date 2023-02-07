#include <malletpch.h>
#include <input/malletinputlayer.h>

#include <managers.h>
#include <imgui/imguiutil.h>
#include <ui/malletui.h>
#include <gl/malletglutil.h>

void SetupInputCallbacks(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, MalletWindowSizeCallback);

	glfwSetKeyCallback(window, MalletKeyCallback);
	glfwSetMouseButtonCallback(window, MalletMouseCallback);
	glfwSetCursorPosCallback(window, MalletMousePosCallback);
}

void MalletKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_DELETE && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		showDemoWindow = !showDemoWindow;
		return;
	}

	inputManager->KeyCallback(key, scancode, action, mods);

	MalletUi::KeyCallback(window, key, scancode, action, mods);
}

void MalletMouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	MalletUi::MouseCallback(window, button, action, mods);
}

void MalletMousePosCallback(GLFWwindow* window, double xPos, double yPos)
{
	MalletUi::MousePosCallback(window, xPos, yPos);
}