#pragma once

#include <ui/docking/dockingleaf.h>
#include <imgui/imguiutil.h>
#include <string>

const static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
ImGuiWindowFlags_NoMove;

class MalletWindow
{
public:
	MalletWindow();
	~MalletWindow();

public:
	std::string name;
	void Draw(){};
};