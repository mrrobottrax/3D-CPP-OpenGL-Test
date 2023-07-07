#pragma once

#include <ui/windows/malletwindow.h>
#include <ui/docking/dockingnode.h>
#include <ui/docking/dockingleaf.h>
#include <ui/docking/dockingsplit.h>

constexpr int splitSelectDistance = 5;

class DockingTree
{
public:
	DockingTree() : pBaseNode()
	{};

	~DockingTree()
	{
		Clear();
	};

private:
	DockingNode* pBaseNode;

public:
	void Clear()
	{
		if (pBaseNode)
			delete pBaseNode;

		pBaseNode = nullptr;
	};

	DockingLeaf* AddLeaf(MalletWindow* pWindow, DockingLeaf* pLeafToSplit,
		SplitDirection = vertical, float ratio = 0.5f, bool flip = false);
	void RemoveNode(DockingNode*);

	void UpdateSize();
	void DrawTree();
	
	DockingNode* GetNodeUnderMouse();
};