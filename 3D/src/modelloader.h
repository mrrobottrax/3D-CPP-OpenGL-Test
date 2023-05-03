#pragma once

#include <meshObject.h>

typedef unsigned int buffer_t;

namespace modelLoader {
	using json = nlohmann::json;

	void LoadModel(MeshObject&, const char*);
	uint32_t ParseUInt32(std::ifstream&);
	uint16_t ParseUInt16(std::ifstream&);
	float	 ParseFloat(std::ifstream&);

	struct Buffer
	{
		buffer_t accessor;
		buffer_t count;
		buffer_t bufferView;
		buffer_t bufferOffset;

		Buffer(json&, buffer_t);
	};
}