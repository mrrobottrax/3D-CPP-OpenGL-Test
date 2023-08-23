#include "malletpch.h"
#include "rawmap.h"
#include <memory/entitymanager.h>
#include <malletarchetypes.h>
#include <world/worldinfo.h>
#include <world/worldutil.h>

using namespace std;

void WriteSectionStart(ofstream& writer, const char*& name)
{
	writer << ":" << name << ":\n\n";
}

void SkipWhitespace(ifstream& reader, bool includeNL = false)
{
	char c;

	reader.read(&c, 1);
	while (reader.good() && ((c == '\n' && includeNL) || c == ' ' || c == '	'))
	{
		reader.read(&c, 1);
	}

	reader.unget();
}

// Check if the section changes
MapSection ReadSection(ifstream& reader)
{
	// Skip whitespace
	SkipWhitespace(reader, true);
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

	SkipWhitespace(reader, true);

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
		SkipWhitespace(reader, true);

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

		SkipWhitespace(reader, false);
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

namespace meshSectionNs
{
	constexpr int numChars = 4;
	constexpr int numSections = 5;
	const char dataSection[numChars] = { 'd', 'a', 't', 'a' };
	const char positionSection[numChars] = { 'p', 'o', 's', ' ' };
	const char vertexSection[numChars] = { 'v', 'e', 'r', 't'};
	const char edgeSection[numChars] = { 'e', 'd', 'g', 'e' };
	const char faceSection[numChars] = { 'f', 'a', 'c', 'e' };

	struct SavedData
	{
		float posX;
		float posY;
		float posZ;

		SavedData(const glm::vec3& position) : posX(position.x),
			posY(position.y), posZ(position.z)
		{};

		SavedData() : posX(0), posY(0), posZ(0)
		{};
	};

	struct SavedPos
	{
		float x;
		float y;
		float z;

		SavedPos(const glm::vec3& vec) : x(vec.x), y(vec.y), z(vec.z)
		{};

		SavedPos() : x(0), y(0), z(0)
		{};
	};

	struct SavedVert
	{
		gSize_t positionIndex;
		float u;
		float v;

		SavedVert(const gSize_t positionIndex, const glm::vec2& uv) :
			positionIndex(positionIndex), u(uv.x), v(uv.y)
		{};

		SavedVert() : positionIndex(0), u(0), v(0)
		{};
	};

	struct SavedEdge
	{
		gSize_t faceIndex;
		gSize_t nextIndex;
		gSize_t prevIndex;
		gSize_t twinIndex;
		gSize_t tailIndex;

		SavedEdge(const gSize_t faceIndex, const gSize_t nextIndex,
			const gSize_t prevIndex, const gSize_t twinIndex, const gSize_t tailIndex) :
			faceIndex(faceIndex), nextIndex(nextIndex), prevIndex(prevIndex),
			twinIndex(twinIndex), tailIndex(tailIndex)
		{};

		SavedEdge() : faceIndex(0), nextIndex(0), prevIndex(0),
			twinIndex(0), tailIndex(0)
		{};
	};

	struct SavedFace
	{
		gSize_t edgeIndex;
		float normalX;
		float normalY;
		float normalZ;

		SavedFace(const gSize_t edgeIndex, const glm::vec3& normal) :
			edgeIndex(edgeIndex), normalX(normal.x), normalY(normal.y), normalZ(normal.z)
		{};

		SavedFace() : edgeIndex(0), normalX(0), normalY(0), normalZ(0)
		{};
	};
}

void WriteMeshData(ofstream& writer)
{
	using namespace meshSectionNs;

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
			EntityPointer e(pChunk, i);
			MalletMesh& mesh = entityManager.GetComponent<MalletMeshComponent>(e).mesh;

			// Add mesh sets to vector
			unordered_set<glm::vec3*> positions;
			unordered_set<mmVertex*> vertices;
			unordered_set<mmHalfEdge*> edges;
			unordered_set<mmFace*> faces;

			mesh.CollectSets(positions, vertices, edges, faces);

			writer.write(dataSection, numChars);
			{
				PositionComponent& position = entityManager.GetComponent<PositionComponent>(e);
				SavedData saved(position.value);
				writer.write(reinterpret_cast<char*>(&saved), sizeof(saved));
			}

			writer.write(positionSection, numChars);
			WriteGsize(writer, static_cast<gSize_t>(positions.size()));
			for (auto it = positions.begin(); it != positions.end(); ++it)
			{
				glm::vec3& vec = **it;
				SavedPos saved(vec);
				writer.write(reinterpret_cast<char*>(&saved), sizeof(saved));
			}

			writer.write(vertexSection, numChars);
			WriteGsize(writer, static_cast<gSize_t>(vertices.size()));
			for (auto it = vertices.begin(); it != vertices.end(); ++it)
			{
				mmVertex& vert = **it;
				gSize_t positionIndex = static_cast<gSize_t>(std::distance(positions.begin(), positions.find(vert.pPosition)));
				SavedVert saved(positionIndex, vert.uv);
				writer.write(reinterpret_cast<char*>(&saved), sizeof(saved));
			}

			writer.write(edgeSection, numChars);
			WriteGsize(writer, static_cast<gSize_t>(edges.size()));
			for (auto it = edges.begin(); it != edges.end(); ++it)
			{
				mmHalfEdge& edge = **it;
				gSize_t faceIndex = static_cast<gSize_t>(std::distance(faces.begin(), faces.find(edge.pFace)));
				gSize_t nextIndex = static_cast<gSize_t>(std::distance(edges.begin(), edges.find(edge.pNext)));
				gSize_t prevIndex = static_cast<gSize_t>(std::distance(edges.begin(), edges.find(edge.pPrev)));
				gSize_t twinIndex = static_cast<gSize_t>(std::distance(edges.begin(), edges.find(edge.pTwin)));
				gSize_t tailIndex = static_cast<gSize_t>(std::distance(vertices.begin(), vertices.find(edge.pTail)));
				SavedEdge saved(faceIndex, nextIndex, prevIndex, twinIndex, tailIndex);
				writer.write(reinterpret_cast<char*>(&saved), sizeof(saved));
			}

			writer.write(faceSection, numChars);
			WriteGsize(writer, static_cast<gSize_t>(faces.size()));
			for (auto it = faces.begin(); it != faces.end(); ++it)
			{
				mmFace& face = **it;
				gSize_t edgeIndex = static_cast<gSize_t>(std::distance(edges.begin(), edges.find(face.pEdge)));
				SavedFace saved(edgeIndex, face.normal);
				writer.write(reinterpret_cast<char*>(&saved), sizeof(saved));
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
	using namespace meshSectionNs;

	SkipWhitespace(reader, true);

	// Load to memory
	char sectionName[numChars];
	SavedPos* savedPositions = nullptr;
	gSize_t posCount = 0;
	SavedVert* savedVerts = nullptr;
	gSize_t vertCount = 0;
	SavedEdge* savedEdges = nullptr;
	gSize_t edgeCount = 0;
	SavedFace* savedFaces = nullptr;
	gSize_t faceCount = 0;

	SavedData data;
	for (int i = 0; i < numSections; ++i)
	{
		reader.read(sectionName, numChars);

		if (!strncmp(sectionName, dataSection, numChars))
		{
			reader.read(reinterpret_cast<char*>(&data), sizeof(SavedData));
		}
		else if (!strncmp(sectionName, positionSection, numChars))
		{
			gSize_t count = ReadGsize(reader);
			savedPositions = new SavedPos[count];
			posCount = count;
			reader.read(reinterpret_cast<char*>(savedPositions), sizeof(SavedPos) * count);
		}
		else if (!strncmp(sectionName, vertexSection, numChars))
		{
			gSize_t count = ReadGsize(reader);
			savedVerts = new SavedVert[count];
			vertCount = count;
			reader.read(reinterpret_cast<char*>(savedVerts), sizeof(SavedVert) * count);
		}
		else if (!strncmp(sectionName, edgeSection, numChars))
		{
			gSize_t count = ReadGsize(reader);
			savedEdges = new SavedEdge[count];
			edgeCount = count;
			reader.read(reinterpret_cast<char*>(savedEdges), sizeof(SavedEdge) * count);
		}
		else if (!strncmp(sectionName, faceSection, numChars))
		{
			gSize_t count = ReadGsize(reader);
			savedFaces = new SavedFace[count];
			faceCount = count;
			reader.read(reinterpret_cast<char*>(savedFaces), sizeof(SavedFace) * count);
		}
		else
		{
			std::cout << "Error reading mesh!\n";
			std::cout << "Location: " << reader.tellg() << endl;
			if (savedPositions)
				delete[] savedPositions;
			if (savedVerts)
				delete[] savedVerts;
			if (savedEdges)
				delete[] savedEdges;
			if (savedFaces)
				delete[] savedFaces;
			return false;
		}
	}

	assert(savedPositions);
	assert(savedVerts);
	assert(savedEdges);
	assert(savedFaces);

	glm::vec3** pPositions = new glm::vec3*[posCount];
	mmVertex** pVertices = new mmVertex*[vertCount];
	mmHalfEdge** pEdges = new mmHalfEdge*[edgeCount];
	mmFace** pFaces = new mmFace*[faceCount];

	for (gSize_t i = 0; i < posCount; ++i)
	{
		pPositions[i] = new glm::vec3;
		pPositions[i]->x = savedPositions[i].x;
		pPositions[i]->y = savedPositions[i].y;
		pPositions[i]->z = savedPositions[i].z;
	}
	for (gSize_t i = 0; i < vertCount; ++i)
	{
		pVertices[i] = new mmVertex();
		pVertices[i]->pPosition = pPositions[savedVerts[i].positionIndex];
		pVertices[i]->uv.x = savedVerts[i].u;
		pVertices[i]->uv.y = savedVerts[i].v;
	}
	for (gSize_t i = 0; i < edgeCount; ++i)
	{
		pEdges[i] = new mmHalfEdge();
	}
	for (gSize_t i = 0; i < faceCount; ++i)
	{
		pFaces[i] = new mmFace();
		pFaces[i]->normal.x = savedFaces[i].normalX;
		pFaces[i]->normal.y = savedFaces[i].normalY;
		pFaces[i]->normal.z = savedFaces[i].normalZ;
		pFaces[i]->pEdge = pEdges[savedFaces[i].edgeIndex];
	}
	for (gSize_t i = 0; i < edgeCount; ++i)
	{
		pEdges[i]->pFace = pFaces[savedEdges[i].faceIndex];
		pEdges[i]->pNext = pEdges[savedEdges[i].nextIndex];
		pEdges[i]->pPrev = pEdges[savedEdges[i].prevIndex];
		pEdges[i]->pTwin = pEdges[savedEdges[i].twinIndex];
		pEdges[i]->pTail = pVertices[savedEdges[i].tailIndex];
	}
	
	EntityManager& em = entityManager;
	Entity e = em.AddEntity(malletMesh);
	EntityPointer ep = em.GetEntityPointer(e);

	em.GetComponent<MeshComponent>(ep).pMesh = nullptr;
	MalletMesh& mesh = em.GetComponent<MalletMeshComponent>(ep).mesh;

	em.GetComponent<PositionComponent>(ep).value = { data.posX, data.posY, data.posZ };
	em.GetComponent<RotationComponent>(ep).value = glm::identity<glm::fquat>();
	em.GetComponent<ScaleComponent>(ep).value = glm::vec3(1);

	mesh.pFirstEdge = pEdges[0];
	mesh.Validate(false);
	mesh.UpdateRenderMesh(ep);

	delete[] savedPositions;
	delete[] savedVerts;
	delete[] savedEdges;
	delete[] savedFaces;
	delete[] pPositions;
	delete[] pVertices;
	delete[] pEdges;
	delete[] pFaces;

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
			SkipWhitespace(reader, true);
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

	worldUtil.UnloadWorld();

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