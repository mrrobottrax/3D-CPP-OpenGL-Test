#include "malletpch.h"
#include "rawmap.h"

using json = nlohmann::json;

namespace versionInfoStrings
{
	constexpr version_t currentFormatVersion = 100;
	constexpr version_t currentEditorVersion = 100;

	const char* versionInfo = "versioninfo";
	const char* formatVersion = "formatversion";
	const char* editorVersion = "editorversion";
	const char* mapVersion = "mapversion";
};

const char worldSection[] = "world";
const char entitySection[] = "entity";

void RawMap::WriteVersionInfo(json& data)
{
	using namespace versionInfoStrings;
	data[versionInfo][formatVersion] = currentFormatVersion;
	data[versionInfo][editorVersion] = currentEditorVersion;
	data[versionInfo][mapVersion] = currentMapVersion;
}

void RawMap::SaveMap(const char* path)
{
	json data;
	WriteVersionInfo(data);

	std::ofstream writer;
	writer.open(path);
	writer << data;
	writer.close();

	++currentMapVersion;
}

bool RawMap::ReadVersionInfo(json& data)
{
	using namespace versionInfoStrings;

	version_t fileVersion = data[versionInfo][formatVersion];
	if (fileVersion != currentFormatVersion)
	{
		std::cout << "ERROR: This map file is version " << fileVersion;
		std::cout << " but this program reads version " << currentFormatVersion << "!\n";

		return false;
	}

	version_t fileEditorVersion = data[versionInfo][editorVersion];
	if (fileEditorVersion != currentEditorVersion)
	{
		std::cout << "ERROR: This map file is for editor version " << fileEditorVersion;
		std::cout << " but this program is version " << currentEditorVersion << "!\n";

		return false;
	}

	currentMapVersion = data[versionInfo][mapVersion] + 1;

	return true;
}

void RawMap::LoadMap(const char* path)
{
	std::ifstream reader;

	reader.open(path);
	json data;
	try
	{
		data = json::parse(reader);
	}
	catch (std::exception e)
	{
		std::cout << "ERROR READING RMAP: " << e.what() << std::endl;
		reader.close();
		return;
	}
	reader.close();

	if (!ReadVersionInfo(data))
		return;
}
