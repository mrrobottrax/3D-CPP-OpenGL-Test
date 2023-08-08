#include "malletpch.h"
#include "filemanager.h"
#include "rawmap.h"

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
		std::cout << "ERROR: " << NFD_GetError() << std::endl;
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
		std::cout << "ERROR: " << NFD_GetError() << std::endl;
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
	std::cout << "Saving map as: " << fullPath << std::endl;

	rawMap.SaveMap(fullPath.c_str());
}

void FileManager::OpenPath(const char* path)
{
	std::cout << "Loading map: " << path << std::endl;

	rawMap.LoadMap(path);
}