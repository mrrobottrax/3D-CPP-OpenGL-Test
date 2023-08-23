#include "malletpch.h"
#include "menubar.h"
#include <gl/glutil.h>
#include <filemanagement/filemanager.h>
#include <world/worldutil.h>

void FileMenu()
{
	if (ImGui::MenuItem("Open", "Ctrl+O"))
		fileManager.OpenDialog();

	if (ImGui::MenuItem("Save As", "Ctrl+S"))
		fileManager.SaveAsDialog();

	if (ImGui::MenuItem("Close"))
		worldUtil.UnloadWorld();

	ImGui::MenuItem("Export", "F9");
}

void MenuBar::Draw()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		FileMenu();

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}
