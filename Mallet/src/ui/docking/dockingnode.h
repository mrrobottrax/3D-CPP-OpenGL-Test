#pragma once

#include <ui/docking/dockingdirection.h>

enum DockingNodeFlags
{
	nodeIsUsed = 1 << 0,
};

struct DockingNode
{
public:
	DockingNode() : flags(0), backIndex(), frontIndex(), direction(), ratio(), absOffset(), parentNodeIndex(-1)
	{

	}

	DockingNode(int parentNodeIndex, int backIndex, int frontIndex, DockingDirection direction, float ratio) :
		backIndex(backIndex), frontIndex(frontIndex), direction(direction), ratio(ratio), flags(0), parentNodeIndex(parentNodeIndex),
		absOffset()
	{
		flags |= DockingNodeFlags::nodeIsUsed;
	}

	~DockingNode()
	{
		
	}

public:
	int flags;

	int parentNodeIndex;

	int backIndex;
	int frontIndex;

	DockingDirection direction;

	float ratio;
	float absOffset;
};