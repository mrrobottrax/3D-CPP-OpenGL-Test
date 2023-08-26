#include "malletpch.h"
#include "malletuisystem.h"

#include <gl/glutil.h>

#include "windows/viewport.h"
#include "windows/toolbar.h"

#include <filemanagement/binarymap.h>

void MalletUiSystem::Init()
{
}

void ExportDialog()
{
	ImGuiViewport* pViewport = ImGui::GetMainViewport();
	ImVec2 pos = pViewport->WorkPos;
	pos.x += pViewport->WorkSize.x / 2;
	pos.y += pViewport->WorkSize.y / 2;
	ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Export", &MalletUiSystem::enableExportDialog, popup_flags);

	if (ImGui::Button("Export"))
		binaryMap.StartExport();

	ImGui::End();
}

void MalletUiSystem::Update()
{
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	menuBar.Draw();

	auto it = windowPs.begin();
	while (it != windowPs.end())
	{
		std::list<MalletWindow*>::iterator oldIt = it;
		++it;

		MalletWindow& window = **oldIt;

		if (!window.IsOpen())
		{
			delete (*oldIt);
			windowPs.erase(oldIt);
			continue;
		}

		window.Draw();
	}

	if (enableExportDialog)
		ExportDialog();
}