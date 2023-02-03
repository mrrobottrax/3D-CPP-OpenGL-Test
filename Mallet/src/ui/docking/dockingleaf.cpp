#include <malletpch.h>
#include <ui/docking/dockingleaf.h>

DockingLeaf::DockingLeaf() : parentNodeIndex(-1), flags(0), absPos(), absSize(), window()
{
}

DockingLeaf::DockingLeaf(int parentNodeIndex, MalletWindow* window) : parentNodeIndex(parentNodeIndex), flags(0), absPos(), absSize(), window(window)
{
	flags |= DockingLeafFlags::leafIsUsed;
}

DockingLeaf::~DockingLeaf()
{
	if (window)
	{
		delete window;
	}
}
