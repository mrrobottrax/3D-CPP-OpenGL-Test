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
	void UpdateTree();
	void DrawTreeDebug();
	void SplitLeaf(int, DockingDirection, float);

private:
	bool IsLeaf(int);
	int GetLiteralLeafIndex(int);
	int AddNode(int, int, DockingDirection, float);
	int AddLeaf(int);
	void DrawNodeRecursiveDebug(int, int, int, int, int);
	void DrawLeafDebug(int, int, int, int, int);
};