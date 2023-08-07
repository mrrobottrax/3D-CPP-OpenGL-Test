#include "malletpch.h"
#include "filemanager.h"

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

void FileManager::SaveToPath(const char* path)
{
	std::string fullPath = path;
	fullPath += '.';
	fullPath += rawMapExtension;
	std::cout << "Saving map as: " << fullPath << std::endl;
}