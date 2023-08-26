#include "malletpch.h"
#include "binarymap.h"
#include <ui/malletuisystem.h>

void BinaryMap::OpenExportDialog()
{
	malletUI.enableExportDialog = true;
}

void BinaryMap::StartExport()
{
	malletUI.enableExportDialog = false;

	DEBUG_LOG("Starting export...");


}