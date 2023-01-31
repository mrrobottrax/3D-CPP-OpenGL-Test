#pragma once

#include <ui/malletwindow.h>

enum DockingLeafFlags
{
	leafIsUsed = 1 << 0,
};

struct DockingLeaf
{
public:
	DockingLeaf() : parentNodeIndex(-1), flags(0), absPos(), absSize(), window(nullptr)
	{

	}

	DockingLeaf(int parentNodeIndex, MalletWindow* window) : parentNodeIndex(parentNodeIndex), flags(0), absPos(), absSize(), window(window)
	{
		flags |= DockingLeafFlags::leafIsUsed;
	}

	~DockingLeaf()
	{
		//delete window;
	}

public:
	int parentNodeIndex;
	int flags;

	int absPos[2];
	int absSize[2];

	MalletWindow* window;
};
