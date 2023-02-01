#pragma once

#include <ui/docking/dockingtree.h>

namespace MalletUi
{
	void Setup();
	void Destroy();

	void PrintTree();
	void DrawTree();
	void RecalculateTree();
	void SplitLeaf(int, DockingDirection, float, MalletWindow*);

	extern int selectedNodeIndex;
	void SelectNode();

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseCallback(GLFWwindow* window, int button, int action, int mods);
	void MousePosCallback(GLFWwindow* window, double xPos, double yPos);
};