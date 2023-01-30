#pragma once

#include <ui/docking/dockingtree.h>

class MalletUiLayer
{
public:
	MalletUiLayer();
	~MalletUiLayer();

private:
	DockingTree tree;

public:
	void DrawTree();
	void SplitLeaf(int, DockingDirection, float);
};