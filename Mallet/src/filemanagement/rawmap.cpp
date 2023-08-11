#include "malletpch.h"
#include "rawmap.h"
#include <memory/entitymanager.h>
#include <malletarchetypes.h>

using namespace std;

void WriteSectionStart(ofstream& writer, const char*& name)
{
	writer << ":" << name << ":\n\n";
}

void SkipWhitespace(ifstream& reader)
{
	char c;

	reader.read(&c, 1);
	while (reader.good() && (c == '\n' || c == ' ' || c == '	'))
	{
		reader.read(&c, 1);
	}

	reader.unget();
}

// Check if the section changes
MapSection ReadSection(ifstream& reader)
{
	// Skip whitespace
	SkipWhitespace(reader);
	auto start = reader.tellg();

	char c;
	reader.read(&c, 1);

	// Make sure it starts with :
	if (c == ':')
	{
		reader.read(&c, 1);

		// Read until newline or :
		vector<char> name;
		while (c != '\n' && c != ':')
		{
			name.push_back(c);
			reader.read(&c, 1);
		}
		name.push_back(NULL);

		// Make sure it ends with :
		if (c == ':')
		{
			// Check for match
			if (!strcmp(name.data(), infoSection))
			{
				return MapSection::info;
			}
			else if (!strcmp(name.data(), entitySection))
			{
				return MapSection::entity;
			}
			else if (!strcmp(name.data(), meshSection))
			{
				return MapSection::mesh;
			}
		}
	}

	reader.seekg(start);
	return none;
}

void WriteObjectStart(ofstream& writer, const char*& name)
{
	writer << name << " {\n";
}

void WriteObjectEnd(ofstream& writer)
{
	writer << "}\n\n";
}

void WriteProperty(ofstream& writer, const char*& name, const char*& value)
{
#ifdef FANCY_PRINTING
	writer << "	" << name << ": " << value << "\n";
#else
	writer << name << ": " << value << "\n";
#endif // FANCY_PRINTING
}

void WriteProperty(ofstream& writer, const char*& name, const char*&& value)
{
#ifdef FANCY_PRINTING
	writer << "	" << name << ": " << value << "\n";
#else
	writer << name << ": " << value << "\n";
#endif // FANCY_PRINTING
}

void WriteVersionProperty(ofstream& writer, const char*& name, version_t value)
{
	WriteProperty(writer, name, to_string(value).c_str());
}

struct GameObject {
	string name;
	unordered_map<string, string> kv; // KeyValues

	void Print()
	{
		std::cout << name << "\n";
		for (auto it = kv.begin(); it != kv.end(); ++it)
		{
			std::cout << "	" << it->first << ": " << it->second << "\n";
		}
	}
};

GameObject ReadObject(ifstream& reader)
{
	GameObject go;

	SkipWhitespace(reader);

	char c;
	reader.read(&c, 1);

	// Read name
	while (reader.good() && c != ' ' && c != openObject)
	{
		go.name += c;
		reader.read(&c, 1);
	}

	// Read till {
	while (reader.good() && c != openObject)
	{
		reader.read(&c, 1);
	}

	while (reader.good())
	{
		SkipWhitespace(reader);

		reader.read(&c, 1);
		if (c == closeObject)
			break;

		// Get key
		vector<char> key;
		while (reader.good() && c != ':')
		{
			key.push_back(c);
			reader.read(&c, 1);
		}
		key.push_back(NULL);

		SkipWhitespace(reader);
		reader.read(&c, 1);

		// Get value
		vector<char> value;
		while (reader.good() && c != '\n')
		{
			value.push_back(c);
			reader.read(&c, 1);
		}
		value.push_back(NULL);

		go.kv.emplace(string(key.data()), string(value.data()));
	}

	return go;
}

namespace versionInfoStrings
{
	const char* versionInfo = "version_info";
	const char* formatVersion = "format_version";
	const char* editorVersion = "editor_version";
};

void WriteVersionInfo(ofstream& writer)
{
	using namespace versionInfoStrings;
	WriteObjectStart(writer, versionInfo);

	WriteVersionProperty(writer, formatVersion, currentFormatVersion);
	WriteVersionProperty(writer, editorVersion, currentEditorVersion);

	WriteObjectEnd(writer);
}

//namespace worldStrings
//{
//	const char* world = "world";
//	const char* meshes = "meshes";
//	const char* meshCount = "meshcount";
//
//	const char* positions = "positions";
//	const char* positionIndex = "positionIndex";
//
//	const char* vertices = "vertices";
//	const char* vertexIndex = "vI";
//
//	const char* edges = "edges";
//	const char* edgeIndex = "eI";
//
//	const char* faces = "faces";
//	const char* faceIndex = "fI";
//
//	const char* uv = "uv";
//};

//struct MeshSetsHolder
//{
//	unordered_set<glm::vec3*> positions;
//	unordered_set<mmVertex*> vertices;
//	unordered_set<mmHalfEdge*> edges;
//	unordered_set<mmFace*> faces;
//};

//void WriteWorldInfo(json& data)
//{
//	using namespace worldStrings;
//
//	char* pData = nullptr;
//
//	// Iterate through all mesh entities
//	ChunkArchetypeElement* pChunkArchetype = entityManager.FindChunkArchetype(malletMesh);
//
//	// No meshes
//	if (!pChunkArchetype)
//		return;
//
//	vector<MeshSetsHolder> meshesVector;
//
//	// Add each mesh to JSON
//	gSize_t meshIndex = 0;
//	for (Chunk* pChunk = pChunkArchetype->pFirstChunk; pChunk; pChunk = pChunk->pNext)
//	{
//		for (gSize_t i = 0; i < pChunk->numberOfEntities; ++i)
//		{
//			MalletMesh& mesh = entityManager.GetComponent<MalletMeshComponent>(EntityPointer(pChunk, i)).mesh;
//
//			// Add mesh sets to vector
//			meshesVector.push_back(MeshSetsHolder());
//			MeshSetsHolder& meshSets = meshesVector.back();
//			mesh.CollectSets(meshSets.positions, meshSets.vertices,
//				meshSets.edges, meshSets.faces);
//
//			++meshIndex;
//		}
//	}
//
//	data[world][meshCount] = meshIndex;
//}

/*--------------------------------------*/

void RawMap::SaveMap(const char* path)
{
	ofstream writer;
	writer.open(path, ios::binary | ios::out | ios::trunc);
	
	WriteSectionStart(writer, infoSection);

	WriteVersionInfo(writer);
	//WriteWorldInfo(writer);

	writer.close();
}

version_t ParseVersion(std::string s)
{
	return std::stoul(s);
}

bool VerifyVersionInfo(GameObject object)
{
	using namespace versionInfoStrings;
	version_t fileFormatVersion = ParseVersion(object.kv[formatVersion]);

	if (fileFormatVersion != currentFormatVersion)
	{
		std::cout << "ERROR: Cannot open format version " << fileFormatVersion << ".\n";
		return false;
	}

	return true;
}

bool InfoSection(ifstream& reader)
{
	GameObject object = ReadObject(reader);
	
	if (!strcmp(object.name.c_str(), versionInfoStrings::versionInfo))
	{
		return VerifyVersionInfo(object);
	}

	return true;
}

void MeshSection(ifstream& reader)
{
	char c;
	reader.read(&c, 1);
}

void EntitySection(ifstream& reader)
{
	char c;
	reader.read(&c, 1);
}

bool LoadMapInternal(ifstream& reader)
{
	MapSection currentSection = none;

	while (reader.good())
	{
		{
			MapSection newSection = ReadSection(reader);
			if (newSection != none)
			{
				currentSection = newSection;
			}
		}

		// Check if right at end of file
		{
			auto lastPos = reader.tellg();
			SkipWhitespace(reader);
			char temp;
			reader.read(&temp, 1);
			if (!reader.good())
				return true;

			reader.seekg(lastPos);
		}

		switch (currentSection)
		{
		case MapSection::info:
			if (!InfoSection(reader))
				return false;
			break;
		case MapSection::entity:
			EntitySection(reader);
			break;
		case MapSection::mesh:
			MeshSection(reader);
			break;
		default:
			break;
		}
	}

	return true;
}

void RawMap::LoadMap(const char* path)
{
	ifstream reader;

	reader.open(path, ios::binary | ios::in);

	if (LoadMapInternal(reader))
	{
		std::cout << "Successfully loaded map\n";
	}
	else
	{
		std::cout << "Could not load map!\n";
	}

	reader.close();
}