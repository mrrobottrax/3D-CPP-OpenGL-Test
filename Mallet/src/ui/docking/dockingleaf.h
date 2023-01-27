#pragma once

#include <ui/malletwindow.h>

enum DockingLeafFlags
{
	leafIsUsed = 1 << 0,
};

struct DockingLeaf
{
public:
	DockingLeaf() : parentNodeIndex(), window(), flags(0)
	{

	}

	DockingLeaf(int parentNodeIndex) : parentNodeIndex(parentNodeIndex), window(), flags(0)
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
};