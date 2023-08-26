#include "malletpch.h"
#include "toolbar.h"

void Toolbar::Draw()
{
	MalletWindowBegin(Toolbar)

	if (ImGui::Button("Select"))
		SetActiveTool(&selectTool);

	if (ImGui::Button("Block"))
		SetActiveTool(&blockTool);

	ImGui::End();
}