#pragma once

#include <ui/docking/dockingleaf.h>
#include <ui/docking/dockingnode.h>

class DockingTree
{
public:
	DockingTree();
	~DockingTree();

public:
	int firstNodeIndex;
	DockingNode nodeArray[20];
	DockingLeaf leafArray[20];

public:
	void UpdateTree();
	void DrawTreeDebug();
};