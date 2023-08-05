#include "malletpch.h"
#include "menubar.h"
#include <gl/glutil.h>

void FileMenu()
{
	ImGui::MenuItem("Save As", "Ctrl+S");
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
