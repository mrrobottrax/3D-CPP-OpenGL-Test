#pragma once

#include <common/types.h>

inline const char rawMapExtension[] = "rmap";
inline const char mapExtension[] = "map";

class FileManager
{
public:
	void SaveAsDialog();
	void OpenDialog();
	void UpdateUI();

	static inline bool enableExportDialog = false;
	void OpenExportDialog();

	void SaveToPath(const char*);
	void Save();
	void OpenPath(const char*);
	std::string GetMapSaveName();

	static bool DirectoryExists(const char* file)
	{
		struct stat sb;
		return (stat(file, &sb) == 0 && !(sb.st_mode & S_IFDIR));
	}

private:
	static inline std::string savepath;

}; inline FileManager fileManager;

inline void WriteUint32(std::ofstream& writer, const uint32_t& value)
{
	writer.write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
}

inline void WriteFloat(std::ofstream& writer, const float& value)
{
	writer.write(reinterpret_cast<const char*>(&value), sizeof(float));
}

inline void WriteUint16(std::ofstream& writer, const uint16_t& value)
{
	writer.write(reinterpret_cast<const char*>(&value), sizeof(uint16_t));
}

inline void WriteGsize(std::ofstream& writer, const gSize_t& value)
{
	writer.write(reinterpret_cast<const char*>(&value), sizeof(gSize_t));
}

inline gSize_t ReadGsize(std::ifstream& reader)
{
	gSize_t value = 0;;
	reader.read(reinterpret_cast<char*>(&value), sizeof(gSize_t));
	return value;
}