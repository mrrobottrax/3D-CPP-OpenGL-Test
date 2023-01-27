#pragma once

#include <ui/docking/dockingdirection.h>

enum DockingNodeFlags
{
	nodeIsUsed = 1 << 0,
};

struct DockingNode
{
public:
	DockingNode() : flags(0), backIndex(), frontIndex(), direction(), ratio()
	{

	}

	DockingNode(int backIndex, int frontIndex, DockingDirection direction, float ratio) :
		backIndex(backIndex), frontIndex(frontIndex), direction(direction), ratio(ratio), flags(0)
	{
		flags |= DockingNodeFlags::nodeIsUsed;
	}

	~DockingNode()
	{
		
	}

public:
	int flags;

	int backIndex;
	int frontIndex;

	DockingDirection direction;

	float ratio;
};