#pragma once

#include <imgui/imguiutil.h>

const static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
ImGuiWindowFlags_NoMove;

struct DockingLeaf;

class MalletWindow
{
public:
	MalletWindow() : selected(false), pLeaf()
	{};

	virtual ~MalletWindow()
	{};

protected:
	DockingLeaf* pLeaf;

public:
	bool selected;

	void SetLeaf(DockingLeaf* pLeaf)
	{
		this->pLeaf = pLeaf;
	}

public:
	virtual void Draw();
	virtual void OnSelect() {};
	virtual void OnDeselect() {};
	virtual void OnResize() {};

	virtual void KeyboardCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) {};
	virtual void MouseCallback(GLFWwindow* pWindow, int button, int action, int mods) {};
	virtual void MousePosCallback(GLFWwindow* pWindow, double xPos, double yPos) {};
	virtual void ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset) {};
};