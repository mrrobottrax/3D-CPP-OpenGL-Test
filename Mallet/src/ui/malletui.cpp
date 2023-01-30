#include <malletpch.h>

#include <ui/malletui.h>
#include <gl/glutil.h>

namespace MalletUi
{
	DockingTree tree;

	void PrintTree()
	{
		tree.PrintTree();
	}

	void DrawTree()
	{
		tree.DrawTree();
	}

	void RecalculateTree()
	{
		tree.RecalculateSizes();
	}

	void SplitLeaf(int leafIndex, DockingDirection dir, float ratio)
	{
		tree.SplitLeaf(leafIndex, dir, ratio);
	}

	int GetNodeRecursive(float mousePosX, float mousePosY)
	{

		return 0;
	}

	int GetNodeToMove()
	{
		double mousePosX, mousePosY;
		glfwGetCursorPos(mainWindow, &mousePosX, &mousePosY);

		float fMousePosX = float(mousePosX);
		float fMousePosY = float(mousePosY);

		return GetNodeRecursive(fMousePosX, fMousePosY);
	}
}