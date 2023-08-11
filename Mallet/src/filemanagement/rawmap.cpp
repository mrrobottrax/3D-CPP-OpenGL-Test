#include "malletpch.h"
#include "rawmap.h"
#include <memory/entitymanager.h>
#include <malletarchetypes.h>
#include <worldinfo.h>

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

namespace worldInfoStrings
{
	const char* worldInfo = "world_info";
	const char* comment = "comment";
};

void WriteWorldInfo(ofstream& writer)
{
	using namespace worldInfoStrings;
	WriteObjectStart(writer, worldInfoStrings::worldInfo);

	WriteProperty(writer, comment, ::worldInfo.comment.c_str());

	WriteObjectEnd(writer);
}

void ReadWorldInfo(GameObject& object)
{
	using namespace worldInfoStrings;
	::worldInfo.comment = object.kv[comment];
}

namespace meshStrings
{
	const char positionSection[4] = { 'p', 'o', 's', ' ' };
	const char vertexSection[4] = { 'v', 'e', 'r', 't'};
	const char edgeSection[4] = { 'e', 'd', 'g', 'e' };
	const char faceSection[4] = { 'f', 'a', 'c', 'e' };
}

void WriteMeshData(ofstream& writer)
{
	using namespace meshStrings;

	// Iterate through all mesh entities
	ChunkArchetypeElement* pChunkArchetype = entityManager.FindChunkArchetype(malletMesh);

	// No meshes
	if (!pChunkArchetype)
		return;

	// Write each mesh
	gSize_t meshIndex = 0;
	for (Chunk* pChunk = pChunkArchetype->pFirstChunk; pChunk; pChunk = pChunk->pNext)
	{
		for (gSize_t i = 0; i < pChunk->numberOfEntities; ++i)
		{
			MalletMesh& mesh = entityManager.GetComponent<MalletMeshComponent>(EntityPointer(pChunk, i)).mesh;

			// Add mesh sets to vector
			unordered_set<glm::vec3*> positions;
			unordered_set<mmVertex*> vertices;
			unordered_set<mmHalfEdge*> edges;
			unordered_set<mmFace*> faces;

			mesh.CollectSets(positions, vertices, edges, faces);

			writer.write(positionSection, 4);
			WriteGsize(writer, positions.size());
			for (auto it = positions.begin(); it != positions.end(); ++it)
			{
				glm::vec3& vec = **it;
				WriteFloat(writer, vec.x);
				WriteFloat(writer, vec.y);
				WriteFloat(writer, vec.z);
			}

			writer.write(vertexSection, 4);
			WriteGsize(writer, vertices.size());
			for (auto it = vertices.begin(); it != vertices.end(); ++it)
			{
				mmVertex& vert = **it;
				ptrdiff_t positionIndex = std::distance(positions.begin(), positions.find(vert.pPosition));
				WriteGsize(writer, positionIndex);
				WriteFloat(writer, vert.uv.x);
				WriteFloat(writer, vert.uv.y);
			}

			writer.write(edgeSection, 4);
			WriteGsize(writer, edges.size());
			for (auto it = edges.begin(); it != edges.end(); ++it)
			{
				mmHalfEdge& edge = **it;
				ptrdiff_t faceIndex = std::distance(faces.begin(), faces.find(edge.pFace));
				ptrdiff_t nextIndex = std::distance(edges.begin(), edges.find(edge.pNext));
				ptrdiff_t prevIndex = std::distance(edges.begin(), edges.find(edge.pPrev));
				ptrdiff_t twinIndex = std::distance(edges.begin(), edges.find(edge.pTwin));
				ptrdiff_t tailIndex = std::distance(vertices.begin(), vertices.find(edge.pTail));
				WriteGsize(writer, faceIndex);
				WriteGsize(writer, nextIndex);
				WriteGsize(writer, prevIndex);
				WriteGsize(writer, twinIndex);
				WriteGsize(writer, tailIndex);
			}

			writer.write(faceSection, 4);
			WriteGsize(writer, faces.size());
			for (auto it = faces.begin(); it != faces.end(); ++it)
			{
				mmFace& face = **it;
				ptrdiff_t edgeIndex = std::distance(edges.begin(), edges.find(face.pEdge));
				WriteGsize(writer, edgeIndex);
				WriteFloat(writer, face.normal.x);
				WriteFloat(writer, face.normal.y);
				WriteFloat(writer, face.normal.z);
			}

			writer.write("\n", 1);
			++meshIndex;
		}
	}
}

/*--------------------------------------*/

void RawMap::SaveMap(const char* path)
{
	ofstream writer;
	writer.open(path, ios::binary | ios::out | ios::trunc);
	
	// info
	WriteSectionStart(writer, infoSection);
	WriteVersionInfo(writer);
	WriteWorldInfo(writer);

	// entity

	// mesh
	WriteSectionStart(writer, meshSection);
	WriteMeshData(writer);

	writer.close();
}

bool InfoSection(ifstream& reader)
{
	GameObject object = ReadObject(reader);
	
	if (!strcmp(object.name.c_str(), versionInfoStrings::versionInfo))
	{
		return VerifyVersionInfo(object);
	}
	else if (!strcmp(object.name.c_str(), worldInfoStrings::worldInfo))
	{
		ReadWorldInfo(object);
	}

	return true;
}

bool MeshSection(ifstream& reader)
{
	using namespace meshStrings;

	SkipWhitespace(reader);

	// Load to memory
	char sectionName[4];
	void* pPosSection = nullptr;
	void* pVertSection = nullptr;
	void* pEdgeSection = nullptr;
	void* pFaceSection = nullptr;
	for (int i = 0; i < 4; ++i)
	{
		reader.read(sectionName, 4);
		gSize_t count = ReadGsize(reader);
		size_t memSize = 0;

		if (!strcmp(sectionName, positionSection))
		{
			memSize = sizeof(float) * 3;
			memSize *= count;
			pPosSection = malloc(memSize);
			reader.read(reinterpret_cast<char*>(pPosSection), memSize);
		}
		else if (!strcmp(sectionName, vertexSection))
		{
			memSize = sizeof(gSize_t) + sizeof(float) * 2;
			memSize *= count;
			pVertSection = malloc(memSize);
			reader.read(reinterpret_cast<char*>(pVertSection), memSize);
		}
		else if (!strcmp(sectionName, edgeSection))
		{
			memSize = sizeof(gSize_t) * 5;
			memSize *= count;
			pEdgeSection = malloc(memSize);
			reader.read(reinterpret_cast<char*>(pEdgeSection), memSize);
		}
		else if (!strcmp(sectionName, faceSection))
		{
			memSize = sizeof(gSize_t) + sizeof(float) * 3;
			memSize *= count;
			pFaceSection = malloc(memSize);
			reader.read(reinterpret_cast<char*>(pFaceSection), memSize);
		}
		else
		{
			std::cout << "Error reading mesh!\n";
			if (pPosSection)
				free(pPosSection);
			if (pVertSection)
				free(pVertSection);
			if (pEdgeSection)
				free(pEdgeSection);
			if (pFaceSection)
				free(pFaceSection);
			return false;
		}
	}

	assert(pPosSection);
	assert(pVertSection);
	assert(pEdgeSection);
	assert(pFaceSection);

	EntityManager& em = entityManager;
	Entity e = em.AddEntity(malletMesh);
	EntityPointer ep = em.GetEntityPointer(e);

	MalletMesh& mesh = em.GetComponent<MalletMeshComponent>(ep).mesh;
	

	free(pPosSection);
	free(pVertSection);
	free(pEdgeSection);
	free(pFaceSection);

	return true;
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
			if (!MeshSection(reader))
				return false;
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