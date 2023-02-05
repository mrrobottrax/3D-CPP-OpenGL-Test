#pragma once

#include <imgui/imguiutil.h>
#include <ui/docking/dockingleaf.h>

const static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
ImGuiWindowFlags_NoMove;

struct DockingLeaf;

class MalletWindow
{
public:
	MalletWindow();
	~MalletWindow();

public:
	bool selected;

public:
	virtual void Draw(DockingLeaf& leaf, int leafIndex);
	inline virtual void OnSelect(DockingLeaf& leaf) {};
	inline virtual void OnDeselect(DockingLeaf& leaf) {};
	inline virtual void OnResize(DockingLeaf& leaf, int windowWidth, int windowHeight) {};

	inline virtual void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {};
	inline virtual void MouseCallback(GLFWwindow* window, int button, int action, int mods) {};
	inline virtual void MousePosCallback(GLFWwindow* window, double xPos, double yPos) {};
};