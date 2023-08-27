#include "malletpch.h"
#include "binarymap.h"
#include <ui/malletuisystem.h>
#include <datareader.h>
#include "filemanager.h"

void WriteMeshes(ofstream& writer)
{

}

void BinaryMap::StartExport()
{
	FileManager::enableExportDialog = false;

	DEBUG_LOG("Starting export...");

	std::string file = (data_folder "maps/") + fileManager.GetMapSaveName() + "." + mapExtension;
	DEBUG_LOG(file);

	ofstream writer;
	writer.open(file, ios::binary | ios::out | ios::trunc);

	WriteMeshes(writer);

	writer.close();
}