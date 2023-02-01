#include <malletpch.h>

#include <gl/malletglutil.h>
#include <ui/malletui.h>

void MalletWindowSizeCallback(GLFWwindow* window, int width, int height)
{
	MalletUi::RecalculateTree();
}