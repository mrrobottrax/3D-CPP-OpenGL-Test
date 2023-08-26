#include "pch.h"
#include "modelLoader.h"
#include <input/console.h>

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
		DEBUG_LOG("Loading model: " << path);

		std::ifstream f;
		f.open(path, std::ios::binary);

		if (f.fail())
		{
			DEBUG_LOG_WARNING("Failed to load model  " << path);
			// TODO: Add default error model
			return;
		}

		// Go to chunk json_length
		f.seekg(12, std::ios::beg);

		uint32_t json_length = ParseUInt32(f);
		
		char* str_json = new char[(size_t)json_length + 1];
		f.seekg(20, std::ios::beg);
		f.read(str_json, json_length);
		str_json[json_length] = NULL;

		json data = json::parse(str_json);

		const buffer_t bufferStart = 20 + json_length + 8;

		// Elements
		Buffer b_indices(data, data["meshes"][0]["primitives"][0]["indices"]);
		mesh.indicesCount = b_indices.count;
		f.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(bufferStart) + b_indices.bufferOffset, std::ios::beg);
		GLshort* indices = new GLshort[b_indices.count];
		for (buffer_t i = 0; i < b_indices.count; ++i)
		{
			indices[i] = ParseUInt16(f);
		}
		mesh.indices = indices;

		// Get buffers
		Buffer b_positions(data, data["meshes"][0]["primitives"][0]["attributes"]["POSITION"]);
		Buffer b_normals(data, data["meshes"][0]["primitives"][0]["attributes"]["NORMAL"]);

		// Create vertex array
		mesh.vertCount = b_positions.count;
		Vertex* vertices = new Vertex[mesh.vertCount];

		// Add positions
		f.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(bufferStart) + b_positions.bufferOffset, std::ios::beg);
		for (int i = 0; i < mesh.vertCount; ++i)
		{
			vertices[i].position[0] = ParseFloat(f);
			vertices[i].position[1] = ParseFloat(f);
			vertices[i].position[2] = ParseFloat(f);
		}

		// Add normals
		f.seekg(static_cast<std::basic_istream<char, std::char_traits<char>>::off_type>(bufferStart) + b_normals.bufferOffset, std::ios::beg);
		for (int i = 0; i < mesh.vertCount; ++i)
		{
			vertices[i].normal[0] = ParseFloat(f);
			vertices[i].normal[1] = ParseFloat(f);
			vertices[i].normal[2] = ParseFloat(f);
		}

		mesh.vertices = vertices;
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