#pragma once

#include <iostream>
#include <engine/meshObject.h>
#include <json.hpp>

namespace modelLoader {
	using json = nlohmann::json;

	void loadModel(MeshObject&, const char*);
	uint32_t parseUInt32(std::ifstream&);
	uint16_t parseUInt16(std::ifstream&);
	float	 parseFloat(std::ifstream&);

	struct Buffer
	{
		int accessor;
		int count;
		int bufferView;
		int bufferOffset;

		Buffer(json&, int);
	};
}