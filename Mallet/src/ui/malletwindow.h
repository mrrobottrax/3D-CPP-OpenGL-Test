#pragma once

#include <imgui/imguiutil.h>
#include <ui/docking/dockingleaf.h>

const static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
ImGuiWindowFlags_NoMove;

struct DockingLeaf;

class MalletWindow
{
public:
	MalletWindow();
	~MalletWindow();

public:
	virtual void Draw(DockingLeaf& leaf, int leafIndex);
};