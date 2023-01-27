#pragma once

#include <ui/docking/dockingleaf.h>
#include <ui/docking/dockingnode.h>

#define MAX_PARTITIONS 20

class DockingTree
{
public:
	DockingTree();
	~DockingTree();

public:
	int firstNodeIndex;
	DockingNode nodeArray[MAX_PARTITIONS];
	DockingLeaf leafArray[MAX_PARTITIONS];

public:
	void UpdateTree();
	void DrawTreeDebug();
};