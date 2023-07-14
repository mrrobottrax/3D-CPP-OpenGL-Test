#include <pch.h>

#include <gl/shaderLoader.h>

namespace shaderLoader {
	const char* LoadShader(const char* path)
	{
		std::ifstream f;
		f.open(path, std::ios::ate | std::ios::binary);

		if (!f.good())
		{
			std::cout << "Failed to load shader file: " << path << "\n";

			std::string path_s(path);
			std::string format = path_s.substr(path_s.find_last_of(".") + 1);

			if (format == "vert")
			{
				char* shaderCopy = reinterpret_cast<char*>(malloc(sizeof(defaultVertexShader)));
				if (shaderCopy != nullptr)
				{
					strcpy_s(shaderCopy, sizeof(defaultVertexShader), defaultVertexShader);
					return shaderCopy;
				}
			}
			else if (format == "frag")
			{
				char* shaderCopy = reinterpret_cast<char*>(malloc(sizeof(defaultFragmentShader)));
				if (shaderCopy != nullptr)
				{
					strcpy_s(shaderCopy, sizeof(defaultFragmentShader), defaultFragmentShader);
					return shaderCopy;
				}
			}

			return nullptr;
		}

		int length = (int)f.tellg();

		char* s = new char[length + 1];
		f.seekg(0, std::ios::beg);
		f.read(s, length);

		s[length] = NULL;

		f.close();

		return s;
	}
}