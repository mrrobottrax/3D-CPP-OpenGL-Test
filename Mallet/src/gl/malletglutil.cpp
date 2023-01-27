#include <malletpch.h>

#include <gl/malletglutil.h>

void MalletWindowSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}