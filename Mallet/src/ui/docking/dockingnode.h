#pragma once

#include <ui/docking/dockingdirection.h>

struct DockingNode
{
	int frontIndex;
	int backIndex;

	DockingDirection direction;

	float ratio;
};