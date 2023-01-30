#pragma once

#include <ui/malletwindow.h>

enum DockingLeafFlags
{
	leafIsUsed = 1 << 0,
};

struct DockingLeaf
{
public:
	DockingLeaf() : parentNodeIndex(), window(), flags(0), absPos(), absSize()
	{

	}

	DockingLeaf(int parentNodeIndex) : parentNodeIndex(parentNodeIndex), window(), flags(0), absPos(), absSize()
	{
		flags |= DockingLeafFlags::leafIsUsed;
	}

	~DockingLeaf()
	{
		delete window;
	}

public:
	MalletWindow* window;

public:
	int parentNodeIndex;
	int flags;

	float absPos[2];
	float absSize[2];
};