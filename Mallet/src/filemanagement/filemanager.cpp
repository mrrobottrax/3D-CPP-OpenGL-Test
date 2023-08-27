#include "malletpch.h"
#include "filemanager.h"
#include "rawmap.h"
#include <input/console.h>
#include <ui/windows/malletwindow.h>
#include "binarymap.h"

void FileManager::SaveAsDialog()
{
	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_SaveDialog(rawMapExtension, "", &outPath);

	if (result == NFD_OKAY)
	{
		SaveToPath(outPath);
	}
	else if (result == NFD_CANCEL)
	{

	}
	else
	{
		DEBUG_LOG_ERROR(NFD_GetError());
	}

	free(outPath);
}

void FileManager::OpenDialog()
{
	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_OpenDialog(rawMapExtension, "", &outPath);

	if (result == NFD_OKAY)
	{
		OpenPath(outPath);
	}
	else if (result == NFD_CANCEL)
	{

	}
	else
	{
		DEBUG_LOG_ERROR(NFD_GetError());
	}

	free(outPath);
}

void ExportDialog()
{
	ImGuiViewport* pViewport = ImGui::GetMainViewport();
	ImVec2 pos = pViewport->WorkPos;
	pos.x += pViewport->WorkSize.x / 2;
	pos.y += pViewport->WorkSize.y / 2;
	ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Export", &FileManager::enableExportDialog, popup_flags);

	if (ImGui::Button("Export"))
		binaryMap.StartExport();

	ImGui::End();
}

void FileManager::UpdateUI()
{
	if (enableExportDialog)
		ExportDialog();
}

void FileManager::SaveToPath(const char* path)
{
	savepath = path;

	// Check if extension needs to be added
	size_t dotPos = savepath.find_last_of('.');
	if (dotPos == std::string::npos || savepath.substr(dotPos + 1).compare(rawMapExtension))
	{
		savepath += '.';
		savepath += rawMapExtension;
	}
	DEBUG_LOG("Saving map as: " << savepath);

	rawMap.SaveMap(savepath.c_str());
}

void FileManager::Save()
{
	if (savepath.empty())
	{
		SaveAsDialog();
		return;
	}

	rawMap.SaveMap(savepath.c_str());
}

void FileManager::OpenPath(const char* path)
{
	DEBUG_LOG("Loading map: " << path);

	savepath = path;
	rawMap.LoadMap(path);
}

std::string FileManager::GetMapSaveName()
{
	if (savepath.empty())
	{
		DEBUG_LOG("Save your map first, doofus!!!");
		// TODO: Add a real warning
		return std::string();
	}

	std::string filename;

	// Remove path
	size_t offset = savepath.find_last_of('\\');
	if (offset != std::string::npos)
	{
		filename = savepath.substr(offset + 1);
	}
	else
	{
		filename = savepath;
	}

	// Remove extension
	offset = filename.find_last_of('.');
	if (offset != std::string::npos)
	{
		filename = filename.substr(0, offset);
	}

	return filename;
}

void FileManager::OpenExportDialog()
{
	enableExportDialog = true;
}