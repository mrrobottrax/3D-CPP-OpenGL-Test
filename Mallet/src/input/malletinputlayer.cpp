#include <malletpch.h>
#include <input/malletinputlayer.h>

#include <imgui/imguiutil.h>
#include <ui/malletuisystem.h>
#include <gl/malletglutil.h>
#include <input/inputsystem.h>

#include <ui/windows/viewport.h>

void SetupInputCallbacks(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, MalletWindowSizeCallback);

	glfwSetKeyCallback(window, MalletKeyCallback);
	glfwSetMouseButtonCallback(window, MalletMouseCallback);
	glfwSetCursorPosCallback(window, MalletMousePosCallback);
	glfwSetScrollCallback(window, MalletScrollCallback);
}

void MalletKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
	// CTRL + DELETE -> ImGui menu
	if (key == GLFW_KEY_DELETE && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		showDemoWindow = !showDemoWindow;
		return;
	}

	// [ or ] -> increase / decrease grid size
	if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
	{
		if (Viewport::baseGridSize > minGridSize)
			Viewport::baseGridSize /= 2;
		return;
	}

	if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
	{
		if (Viewport::baseGridSize < maxGridSize)
			Viewport::baseGridSize *= 2;
		return;
	}

	inputSystem.KeyCallback(key, scancode, action, mods);

	malletUI.KeyCallback(pWindow, key, scancode, action, mods);
}

void MalletMouseCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
	inputSystem.MouseCallback(button, action);

	malletUI.MouseCallback(pWindow, button, action, mods);
}

void MalletMousePosCallback(GLFWwindow* pWindow, double xPos, double yPos)
{
	malletUI.MousePosCallback(pWindow, xPos, yPos);
}

void MalletScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset)
{
	inputSystem.ScrollCallback(xOffset, yOffset);

	malletUI.ScrollCallback(pWindow, xOffset, yOffset);
}