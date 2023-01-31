#include "malletpch.h"
#include "viewport3d.h"

void Viewport3D::Draw(DockingLeaf& leaf, int leafIndex)
{
	ImGui::SetNextWindowPos(ImVec2(float(leaf.absPos[0]), float(leaf.absPos[1])));
	ImGui::SetNextWindowSize(ImVec2(float(leaf.absSize[0]), float(leaf.absSize[1])));

	std::string name = std::to_string(leafIndex);

	ImGui::Begin(name.c_str(), (bool*)0, window_flags);
	ImGui::End();
}
