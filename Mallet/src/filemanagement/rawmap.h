#pragma once

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