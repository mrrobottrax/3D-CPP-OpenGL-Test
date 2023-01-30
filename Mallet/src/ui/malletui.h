#pragma once

#include <ui/docking/dockingtree.h>

namespace MalletUi
{
	extern DockingTree tree;

	void DrawTree();
	void SplitLeaf(int, DockingDirection, float);
};