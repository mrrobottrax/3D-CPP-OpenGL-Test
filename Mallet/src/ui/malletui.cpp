#include <malletpch.h>

#include <ui/malletui.h>

namespace MalletUi
{
	DockingTree tree;

	void DrawTree()
	{
		tree.DrawTree();
	}

	void SplitLeaf(int leafIndex, DockingDirection dir, float ratio)
	{
		tree.SplitLeaf(leafIndex, dir, ratio);
	}
}