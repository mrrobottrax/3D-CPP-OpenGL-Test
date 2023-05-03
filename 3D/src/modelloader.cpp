#include <pch.h>

#include <modelLoader.h>

namespace modelLoader {

	Buffer::Buffer(json& data, buffer_t accessor)
	{
		Buffer::accessor = accessor;
		count = data["accessors"][accessor]["count"];
		bufferView = data["accessors"][accessor]["bufferView"];
		bufferOffset = data["bufferViews"][bufferView]["byteOffset"];
	}

	void LoadModel(MeshObject& mesh, const char* path)
	{
		std::cout << "Loading model: " << path << "\n";

		std::ifstream f;
		f.open(path, std::ios::binary);

		if (f.fail())
		{
			std::cout << "Failed to load model  " << path << "\n";
			return;
		}

		// Go to chunk json_length
		f.seekg(12, std::ios::beg);

		uint32_t json_length = ParseUInt32(f);
		
		char* str_json = new char[json_length + 1];
		f.seekg(20, std::ios::beg);
		f.read(str_json, json_length);
		str_json[json_length] = NULL;

		json data = json::parse(str_json);

		const int bufferStart = 20 + json_length + 8;

		// Elements
		Buffer b_indices(data, data["meshes"][0]["primitives"][0]["indices"]);
		mesh.indicesCount = b_indices.count;
		f.seekg(bufferStart + b_indices.bufferOffset, std::ios::beg);
		GLshort* indices = new GLshort[b_indices.count];
		for (buffer_t i = 0; i < b_indices.count; ++i)
		{
			indices[i] = ParseUInt16(f);
		}
		mesh.indices = indices;

		// Positions
		Buffer b_positions(data, data["meshes"][0]["primitives"][0]["attributes"]["POSITION"]);
		mesh.vertCount = b_positions.count * 3;
		f.seekg(bufferStart + b_positions.bufferOffset, std::ios::beg);
		float* positions = new float[mesh.vertCount];
		for (int i = 0; i < mesh.vertCount; ++i)
		{
			positions[i] = ParseFloat(f);
		}
		mesh.verts = positions;

		// Normals
		Buffer b_normals(data, data["meshes"][0]["primitives"][0]["attributes"]["NORMAL"]);
		mesh.normalCount = b_normals.count * 3;
		f.seekg(bufferStart + b_normals.bufferOffset, std::ios::beg);
		float* normals = new float[mesh.normalCount];
		for (int i = 0; i < mesh.normalCount; ++i)
		{
			normals[i] = ParseFloat(f);
		}
		mesh.normals = normals;

		mesh.GenBuffers();

		delete[] str_json;
		f.close();
	}

	uint32_t ParseUInt32(std::ifstream& f)
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

	uint16_t ParseUInt16(std::ifstream& f)
	{
		unsigned int num = 0;
		char* p = (char*)&num;

		for (int i = 0; i < 2; ++i)
		{
			*p = f.get();
			p += 1;
		}
		return num;
	}

	float ParseFloat(std::ifstream& f)
	{
		float num = 0;
		char* p = (char*)&num;

		for (int i = 0; i < 4; ++i)
		{
			*p = f.get();
			p += 1;
		}
		return num;
	}
}