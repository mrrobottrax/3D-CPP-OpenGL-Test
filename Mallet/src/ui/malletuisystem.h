#pragma once

#include <systems/system.h>
#include <ui/docking/dockingtree.h>
#include "menubar.h"

class MalletUiSystem : public System
{
public:
	MalletUiSystem() : pSelectedNode(nullptr)
	{};

	~MalletUiSystem()
	{};

	void Init() override;
	void Update() override;

	void WindowSizeCallback(GLFWwindow*, int width, int height);

private:
	MenuBar menuBar;
	DockingTree tree;
	DockingNode* pSelectedNode;

public:
	void SelectNodeUnderMouse(bool selectSplits = false);
	void DeselectNode();

	void KeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
	void MouseCallback(GLFWwindow* pWindow, int button, int action, int mods);
	void MousePosCallback(GLFWwindow* pWindow, double xPos, double yPos);
	void ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset);

}; inline MalletUiSystem malletUI;