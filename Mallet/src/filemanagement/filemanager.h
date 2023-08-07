#pragma once

inline const char rawMapExtension[] = "rmap";

class FileManager
{
public:
	void SaveAsDialog();
	void SaveToPath(const char*);
}; inline FileManager fileManager;