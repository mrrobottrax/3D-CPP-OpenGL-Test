#include "malletpch.h"
#include "malletuisystem.h"

#include <gl/glutil.h>

#include "windows/viewport.h"
#include "windows/toolbar.h"

#include <filemanagement/binarymap.h>
#include <filemanagement/filemanager.h>

void MalletUiSystem::Init()
{
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

	fileManager.UpdateUI();
}