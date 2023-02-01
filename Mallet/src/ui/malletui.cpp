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

	void SelectNode()
	{
		double mousePosX, mousePosY;
		glfwGetCursorPos(mainWindow, &mousePosX, &mousePosY);

		float fMousePosX = float(mousePosX);
		float fMousePosY = float(mousePosY);

		selectedNodeIndex = tree->SelectNode(fMousePosX, fMousePosY);
		std::cout << selectedNodeIndex << "\n";
	}
}