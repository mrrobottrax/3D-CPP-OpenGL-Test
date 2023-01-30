#pragma once

#include <ui/docking/dockingtree.h>

namespace MalletUi
{
	void DrawTree();
	void RecalculateTree();
	void SplitLeaf(int, DockingDirection, float);
	int GetNodeToMove();
};