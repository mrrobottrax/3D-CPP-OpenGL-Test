#pragma once

#include <engine/meshObject.h>

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