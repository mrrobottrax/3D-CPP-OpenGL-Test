#include <malletpch.h>

#include <ui/malletui.h>
#include <gl/glutil.h>

namespace MalletUi
{
	DockingTree* tree;

	void Setup()
	{
		tree = new DockingTree();
	}

	void Destroy()
	{
		delete tree;
	}

	void PrintTree()
	{
		tree->PrintTree();
	}

	void DrawTree()
	{
		tree->DrawTree();
	}

	void RecalculateTree()
	{
		tree->RecalculateSizes();
	}

	void SplitLeaf(int leafIndex, DockingDirection dir, float ratio, MalletWindow* window)
	{
		tree->SplitLeaf(leafIndex, dir, ratio, window);
	}

	int selectedNodeIndex;
	bool selected = false;

	void DeselectNode()
	{
		selected = false;

		if (tree->IsLeaf(selectedNodeIndex))
		{
			DockingLeaf& leaf = tree->leafArray[abs(selectedNodeIndex) - 1];
			if (leaf.window != nullptr)
			{
				leaf.window->selected = false;
				leaf.window->OnDeselect(leaf);
			}
		}
	}

	void SelectNode()
	{
		double mousePosX, mousePosY;
		glfwGetCursorPos(mainWindow, &mousePosX, &mousePosY);

		float fMousePosX = float(mousePosX);
		float fMousePosY = float(mousePosY);

		DeselectNode();

		selectedNodeIndex = tree->SelectNode(fMousePosX, fMousePosY);
		selected = true;

		if (tree->IsLeaf(selectedNodeIndex))
		{
			DockingLeaf& leaf = tree->leafArray[abs(selectedNodeIndex) - 1];
			if (leaf.window != nullptr)
			{
				leaf.window->selected = true;
				leaf.window->OnSelect(leaf);
			}
		}
	}

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			DeselectNode();
			return;
		}

		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			int mode = glfwGetInputMode(mainWindow, GLFW_CURSOR);
			if (mode != GLFW_CURSOR_DISABLED)
				SelectNode();
		}

		if (selectedNodeIndex < 0 && selected)
		{
			DockingLeaf& leaf = tree->leafArray[abs(selectedNodeIndex) - 1];

			if (leaf.window != nullptr)
				leaf.window->KeyboardCallback(window, key, scancode, action, mods);
		}
	}
	void MouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
		{
			SelectNode();
			return;
		}

		if (selectedNodeIndex < 0 && selected)
		{
			DockingLeaf& leaf = tree->leafArray[abs(selectedNodeIndex) - 1];

			if (leaf.window != nullptr)
				leaf.window->MouseCallback(window, button, action, mods);
		}
	}

	void MousePosCallback(GLFWwindow* window, double xPos, double yPos)
	{
		if (selectedNodeIndex < 0 && selected)
		{
			DockingLeaf& leaf = tree->leafArray[abs(selectedNodeIndex) - 1];

			if (leaf.window != nullptr)
				leaf.window->MousePosCallback(window, xPos, yPos);
		}
	}
}