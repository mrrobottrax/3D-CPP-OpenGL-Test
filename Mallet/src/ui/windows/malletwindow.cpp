#include "malletpch.h"
#include "malletwindow.h"

#include <ui/docking/dockingleaf.h>

void MalletWindow::Draw()
{
	ImGui::SetNextWindowPos(ImVec2(float(pLeaf->position[0]), float(pLeaf->position[1])));
	ImGui::SetNextWindowSize(ImVec2(float(pLeaf->size[0]), float(pLeaf->size[1])));

	std::string name = std::to_string((uintptr_t)pLeaf);

	ImGui::Begin(name.c_str(), (bool*)0, window_flags);
	ImGui::End();
}