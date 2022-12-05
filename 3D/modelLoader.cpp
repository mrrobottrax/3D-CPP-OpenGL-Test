#include <modelLoader.h>
#include <json/json.hpp>
#include <fstream>

namespace modelLoader {

	using json = nlohmann::json;

	void loadModel(MeshObject& mesh, const char* path)
	{
		std::cout << "Loading model: " << path << "\n";

		std::ifstream f;
		f.open(path);

		if (f.fail())
		{
			std::cout << "Failed to load model  " << path << "\n";
			return;
		}

		// Go to chunk length
		f.seekg(12, std::ios::beg);

		uint32_t length = parseUInt32(f);
		
		char* str_json = new char[length + 1];
		f.seekg(20, f.beg);
		f.read(str_json, length);
		str_json[length] = NULL;

		json data = json::parse(str_json);

		uint16_t indicesAccessor = data["meshes"][0]["primitives"][0]["indices"];
		uint16_t positionAccessor = data["meshes"][0]["primitives"][0]["attributes"]["POSITION"];

		mesh.indicesCount = data["accessors"][indicesAccessor]["count"];

		delete[] str_json;
		f.close();
	}

	uint32_t parseUInt32(std::ifstream& f)
	{
		unsigned int num = 0;
		char* p = (char*) &num;

		for (int i = 0; i < 4; ++i)
		{
			*p = f.get();
			p += 1;
		}

		return num;
	}
}