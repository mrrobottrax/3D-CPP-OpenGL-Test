#pragma once

#include <ui/malletwindow.h>
#include <ui/docking/dockingleaf.h>

class Viewport3D : public MalletWindow
{
public:
	Viewport3D()
	{

	};

	~Viewport3D()
	{

	};

	void Draw(DockingLeaf& leaf, int leafIndex);
	void OnResize(DockingLeaf& leaf, int windowWidth, int windowHeight);
};