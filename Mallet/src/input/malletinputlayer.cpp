#include <malletpch.h>
#include <input/malletinputlayer.h>

#include <imgui/imguiutil.h>
#include <ui/malletui.h>
#include <gl/malletglutil.h>
#include <input/inputmanager.h>

//Button mallet_buttons[MAX_BUTTONS_MALLET];
//
//void PanDown(Console& console) { inputManager->ButtonUp(mallet_buttons[Pan]); };
//void PanUp(Console& console) { inputManager->ButtonUp(mallet_buttons[Pan]); };

void SetupInputCallbacks(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, MalletWindowSizeCallback);

	glfwSetKeyCallback(window, MalletKeyCallback);
	glfwSetMouseButtonCallback(window, MalletMouseCallback);
	glfwSetCursorPosCallback(window, MalletMousePosCallback);
}

//void AddInputCommands()
//{
//	inputManager->console.AddCommand("+pan", PanDown);
//	inputManager->console.AddCommand("-pan", PanUp);
//
//	inputManager->BindKey('r', "+pan");
//}

void MalletKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_DELETE && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		showDemoWindow = !showDemoWindow;
		return;
	}

	inputManager.KeyCallback(key, scancode, action, mods);

	MalletUi::KeyCallback(window, key, scancode, action, mods);
}

void MalletMouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	inputManager.MouseCallback(button, action);

	MalletUi::MouseCallback(window, button, action, mods);
}

void MalletMousePosCallback(GLFWwindow* window, double xPos, double yPos)
{
	MalletUi::MousePosCallback(window, xPos, yPos);
}
