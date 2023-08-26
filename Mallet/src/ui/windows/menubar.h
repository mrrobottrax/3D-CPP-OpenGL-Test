#pragma once

constexpr ImGuiWindowFlags menuBarFlags = ImGuiWindowFlags_NoDecoration |
ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
ImGuiWindowFlags_NoMove;

constexpr int menuBarSize = 18;

class MenuBar
{
public:
	void Draw();
};