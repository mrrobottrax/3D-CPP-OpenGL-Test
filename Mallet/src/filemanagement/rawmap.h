#pragma once

#include <common/types.h>

#define FANCY_PRINTING

typedef unsigned int version_t;
constexpr version_t currentFormatVersion = 100;
constexpr version_t currentEditorVersion = 100;

constexpr char openObject = '{';
constexpr char closeObject = '}';

enum MapSection
{
	none = 0,
	info,
	entity,
	mesh
};

inline const char* infoSection = "info";
inline const char* entitySection = "entity";
inline const char* meshSection = "mesh";

class RawMap
{
public:
	void SaveMap(const char*);
	void LoadMap(const char*);
}; inline RawMap rawMap;

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