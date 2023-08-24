#pragma once

#include <common/types.h>

inline const char rawMapExtension[] = "rmap";

class FileManager
{
public:
	void SaveAsDialog();
	void OpenDialog();

	void SaveToPath(const char*);
	void OpenPath(const char*);
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