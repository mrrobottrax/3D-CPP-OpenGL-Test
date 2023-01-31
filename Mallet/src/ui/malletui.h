#pragma once

#include <ui/docking/dockingtree.h>

namespace MalletUi
{
	void PrintTree();
	void DrawTree();
	void RecalculateTree();
	void SplitLeaf(int, DockingDirection, float, MalletWindow*);
	int GetNodeToMove();
};