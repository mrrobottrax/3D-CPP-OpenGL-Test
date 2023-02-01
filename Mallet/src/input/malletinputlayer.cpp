#include <malletpch.h>
#include <input/malletinputlayer.h>

#include <inputmanager.h>
#include <imgui/imguiutil.h>
#include <ui/malletui.h>

void MalletKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	MalletUi::KeyCallback(window, key, scancode, action, mods);
}

void MalletMouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	MalletUi::MouseCallback(window, button, action, mods);
}