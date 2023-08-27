#include "malletpch.h"
#include "menubar.h"
#include <gl/glutil.h>
#include <filemanagement/filemanager.h>
#include <world/worldutil.h>
#include <filemanagement/binarymap.h>
#include <ui/malletuisystem.h>

void FileMenu()
{
	if (ImGui::MenuItem("Open", "Ctrl+O"))
		fileManager.OpenDialog();

	if (ImGui::MenuItem("Save", "Ctrl+S"))
		fileManager.Save();

	if (ImGui::MenuItem("Save As.."))
		fileManager.SaveAsDialog();

	if (ImGui::MenuItem("Close"))
		worldUtil.UnloadWorld();

	ImGui::Separator();

	if (ImGui::MenuItem("Export", "F9"))
		fileManager.OpenExportDialog();
}

inline void AddWindow(MalletWindow* pMalletWindow)
{
	malletUI.windowPs.push_back(pMalletWindow);
}

void WindowMenu()
{
	if (ImGui::MenuItem("Toolbar"))
		AddWindow(new Toolbar());

	if (ImGui::MenuItem("Viewport"))
		AddWindow(new Viewport());
}

void MenuBar::Draw()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		FileMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window"))
	{
		WindowMenu();
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}
