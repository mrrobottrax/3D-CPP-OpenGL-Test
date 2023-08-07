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

	std::ofstream writer;
	writer.open(fullPath, std::ios::binary);
	
	writer << fileSignature;
	Write(writer, rmapVersion);

	writer.close();
}

void FileManager::OpenPath(const char* path)
{
	std::string fullPath = path;
	std::cout << "Loading map: " << fullPath << std::endl;

	std::ifstream reader;
	reader.open(fullPath, std::ios::binary);

	// Check if signature is correct (no null byte)
	const size_t bytes = sizeof(rawMapExtension) - 1;
	char sig[bytes];
	reader.read(sig, bytes);

	if (std::strncmp(sig, fileSignature, bytes))
	{
		std::cout << "ERROR: This is not a raw map file!\n";
		return;
	}

	version_t version = ReadVersion(reader);
	if (version != rmapVersion)
	{
		std::cout << "ERROR: This map file is version " << version;
		std::cout << " but this program reads version " << rmapVersion << "!\n";
	}
}