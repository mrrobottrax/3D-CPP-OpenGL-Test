#pragma once
#include <string>
#include <GLFW/glfw3.h>
#include <ui/windows/viewport.h>

class MalletTool
{
public:
	static inline bool glInit = false;

	virtual void Draw(Viewport*) {};

	virtual void KeyboardCallback(Viewport*, GLFWwindow* pWindow, int key, int scancode, int action, int mods) {};
	virtual void MouseCallback(Viewport*, GLFWwindow* pWindow, int button, int action, int mods) {};
	virtual void MousePosCallback(Viewport*, GLFWwindow* pWindow, double xPos, double yPos) {};
	virtual void ScrollCallback(Viewport*, GLFWwindow* pWindow, double xOffset, double yOffset) {};
};