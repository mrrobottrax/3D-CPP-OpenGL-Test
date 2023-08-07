#include "malletpch.h"
#include "menubar.h"
#include <gl/glutil.h>
#include <filemanagement/filemanager.h>

void FileMenu()
{
	if (ImGui::MenuItem("Open", "Ctrl+O"))
		fileManager.OpenDialog();

	if (ImGui::MenuItem("Save As", "Ctrl+S"))
		fileManager.SaveAsDialog();

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
