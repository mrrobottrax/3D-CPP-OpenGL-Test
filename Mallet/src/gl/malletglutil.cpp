#include <malletpch.h>

#include <gl/malletglutil.h>
#include <ui/malletuisystem.h>

void MalletWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
	malletUI.WindowSizeCallback(pWindow, width, height);
}