#pragma once

#include <imgui.h>

struct GLFWwindow;

constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoSavedSettings;
constexpr ImGuiWindowFlags popup_flags = window_flags | ImGuiWindowFlags_NoResize 
| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

#define MalletWindowBeginAndPop(name, stylesToPop) \
std::ostringstream oss; \
oss << this; \
std::string address_a = oss.str(); \
if (!ImGui::Begin((#name "##" + address_a).c_str(), &open, window_flags)) \
{ \
	ImGui::End(); \
	ImGui::PopStyleVar(stylesToPop); \
	return; \
}

#define MalletWindowBegin(name) \
std::ostringstream oss; \
oss << this; \
std::string address_a = oss.str(); \
if (!ImGui::Begin((#name "##" + address_a).c_str(), &open, window_flags)) \
{ \
	ImGui::End(); \
	return; \
}

class MalletWindow
{
public:
	MalletWindow() : open(true)
	{};

	virtual ~MalletWindow()
	{};

protected:
	bool open;

public:
	virtual void Draw();

	bool IsOpen()
	{
		return open;
	}

	virtual void KeyboardCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) {};
	virtual void MouseCallback(GLFWwindow* pWindow, int button, int action, int mods) {};
	virtual void MousePosCallback(GLFWwindow* pWindow, double xPos, double yPos) {};
	virtual void ScrollCallback(GLFWwindow* pWindow, double xOffset, double yOffset) {};
};