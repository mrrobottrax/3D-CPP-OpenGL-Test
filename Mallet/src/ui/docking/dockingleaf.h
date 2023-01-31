#pragma once

#include <ui/malletwindow.h>

enum DockingLeafFlags
{
	leafIsUsed = 1 << 0,
};

struct DockingLeaf
{
public:
	DockingLeaf() : parentNodeIndex(-1), flags(0), absPos(), absSize()
	{

	}

	DockingLeaf(int parentNodeIndex) : parentNodeIndex(parentNodeIndex), flags(0), absPos(), absSize()
	{
		flags |= DockingLeafFlags::leafIsUsed;
	}

	~DockingLeaf()
	{
		//delete window;
	}

public:
	//MalletWindow* window;

public:
	int parentNodeIndex;
	int flags;

	int absPos[2];
	int absSize[2];
};