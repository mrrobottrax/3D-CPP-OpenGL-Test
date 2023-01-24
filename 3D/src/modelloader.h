#pragma once

#include <meshObject.h>

namespace modelLoader {
	using json = nlohmann::json;

	void LoadModel(MeshObject&, const char*);
	uint32_t ParseUInt32(std::ifstream&);
	uint16_t ParseUInt16(std::ifstream&);
	float	 ParseFloat(std::ifstream&);

	struct Buffer
	{
		int accessor;
		int count;
		int bufferView;
		int bufferOffset;

		Buffer(json&, int);
	};
}