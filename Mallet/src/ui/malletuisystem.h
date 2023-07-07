#pragma once

#include <systems/system.h>
#include <ui/docking/dockingtree.h>

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
	DockingTree tree;
	DockingNode* pSelectedNode;

public:
	void SelectNodeUnderMouse();
	void DeselectNode();

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseCallback(GLFWwindow* window, int button, int action, int mods);
	void MousePosCallback(GLFWwindow* window, double xPos, double yPos);

}; inline MalletUiSystem malletUI;