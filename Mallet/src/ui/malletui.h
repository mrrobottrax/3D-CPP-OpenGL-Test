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

	void SelectNode();
};