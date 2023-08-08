#pragma once

inline const char rawMapExtension[] = "rmap";

class FileManager
{
public:
	void SaveAsDialog();
	void OpenDialog();

	void SaveToPath(const char*);
	void OpenPath(const char*);
}; inline FileManager fileManager;