#pragma once

#include <ui/windows/malletwindow.h>

enum DockingLeafFlags
{
	leafIsUsed = 1 << 0,
};

class MalletWindow;

struct DockingLeaf
{
public:
	DockingLeaf();
	DockingLeaf(int parentNodeIndex, MalletWindow* window);

	~DockingLeaf();

public:
	int parentNodeIndex;
	int flags;

	int absPos[2];
	int absSize[2];

	MalletWindow* window;
};
