#pragma once

#include <system.h>
#include <forward_list>
#include <typeindex>

// Each chunk is 16kB
const unsigned short chunkSize = 16384;

struct Archetype
{
	std::type_index* components;
	unsigned short componentCount;

	friend bool operator==(const Archetype& lhs, const Archetype& rhs)
	{
		if (lhs.componentCount != rhs.componentCount)
			return false;

		return lhs.components == rhs.components;
	}
};

struct ChunkElement
{
	ChunkElement* next;
	unsigned short numberOfEntities;
};

// Points to a linked list of chunks, each of the same archetype
struct ChunkArchetypeElement
{
	ChunkArchetypeElement* next;
	ChunkElement* firstChunk;

	Archetype archetype;
};

class EntitySystem : public System
{
public:
	EntitySystem();
	~EntitySystem();

	void update() override;

	void addEntity(std::type_index*);
	void createChunk(Archetype*);
	void createChunkArchetype(Archetype*);
	ChunkArchetypeElement* findChunkArchetype(Archetype*);

private:
	ChunkArchetypeElement* chunkArchetypeList;
	ChunkArchetypeElement* chunkArchetypeListLast;
};