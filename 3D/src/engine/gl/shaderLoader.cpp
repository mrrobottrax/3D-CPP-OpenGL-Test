#include <pch.h>

#include <engine/gl/shaderLoader.h>

namespace shaderLoader {
	const char* loadShader(const char* path)
	{
		std::ifstream f;
		f.open(path, std::ios::ate | std::ios::binary);

		int length = (int)f.tellg();

		char* s = new char[length + 1];
		f.seekg(0, std::ios::beg);
		f.read(s, length);

		s[length] = NULL;

		f.close();

		return s;
	}
}