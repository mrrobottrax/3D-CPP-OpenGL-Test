#pragma once

#include <iostream>
#include <meshObject.h>

namespace modelLoader {
	void loadModel(MeshObject&, const char*);
	unsigned int parseUInt32(std::ifstream&);
}