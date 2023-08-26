#include "malletpch.h"
#include "filemanager.h"
#include "rawmap.h"
#include <input/console.h>

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

void FileManager::SaveToPath(const char* path)
{
	std::string fullPath = path;

	// Check if extension needs to be added
	size_t dotPos = fullPath.find_last_of('.');
	if (dotPos == std::string::npos || fullPath.substr(dotPos + 1).compare(rawMapExtension))
	{
		fullPath += '.';
		fullPath += rawMapExtension;
	}
	DEBUG_LOG("Saving map as: " << fullPath);

	rawMap.SaveMap(fullPath.c_str());
}

void FileManager::OpenPath(const char* path)
{
	DEBUG_LOG("Loading map: " << path);

	rawMap.LoadMap(path);
}