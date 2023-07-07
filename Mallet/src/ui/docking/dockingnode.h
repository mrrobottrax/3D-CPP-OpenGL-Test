#pragma once

struct DockingSplit;

struct DockingNode
{
public:
	DockingNode() : isLeaf(), pParentSplit()
	{};

	virtual ~DockingNode() {};

public:
	bool isLeaf = false;
	DockingSplit* pParentSplit;
};