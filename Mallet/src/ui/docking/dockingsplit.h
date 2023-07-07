#pragma once

#include "dockingnode.h"

enum SplitDirection
{
	horizontal,
	vertical
};

struct DockingSplit : public DockingNode
{
public:
	DockingSplit() : direction(), splitRatio(), splitPosition(),
		back(), front(), min(), max()
	{
		isLeaf = false;
	};

	~DockingSplit()
	{
		if (back)
			delete back;

		if (front)
			delete front;
	};

public:
	SplitDirection direction;

	float splitRatio;
	int splitPosition;
	int min;
	int max;

	DockingNode* back;
	DockingNode* front;
};