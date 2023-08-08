#pragma once

typedef unsigned int version_t;

class RawMap
{
public:
	version_t currentMapVersion = 0;

public:
	void WriteVersionInfo(nlohmann::json& data);
	bool ReadVersionInfo(nlohmann::json& data);

	void SaveMap(const char*);
	void LoadMap(const char*);
}; inline RawMap rawMap;