#pragma once

#include "dockingnode.h"
#include <ui/windows/malletwindow.h>

struct DockingLeaf : public DockingNode
{
public:
	DockingLeaf() : position(), size(), pWindow()
	{
		isLeaf = true;
	};

	DockingLeaf(MalletWindow* pWindow) : position(), size(), pWindow(pWindow)
	{
		isLeaf = true;
		pWindow->SetLeaf(this);
	};

	~DockingLeaf()
	{
		if (pWindow)
		{
			delete pWindow;
		}
	};

public:
	int position[2];
	int size[2];

	MalletWindow* pWindow;
};
