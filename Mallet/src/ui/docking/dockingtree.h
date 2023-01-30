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
	int rootNode;
	DockingNode nodeArray[MAX_PARTITIONS];
	DockingLeaf leafArray[MAX_PARTITIONS];

public:
	void PrintTree();
	void DrawTree();
	int SplitLeaf(int, DockingDirection, float);
	void RecalculateSizes();

private:
	bool IsLeaf(int);
	int AddNode(int, int, int, DockingDirection, float);
	int AddLeaf(int);
	void DrawNodeRecursive(int, float, float, float, float);
	void DrawLeafDebug(int, float, float, float, float);
	void DrawLeaf(int, float, float, float, float);
};