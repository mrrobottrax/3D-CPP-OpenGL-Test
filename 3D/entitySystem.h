#pragma once

#include <system.h>
#include <forward_list>
#include <typeindex>
#include <entityArchetype.h>

// Each chunk is 16kB
const unsigned short chunkSize = 16384;

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

	EntityArchetype archetype;
};

class EntitySystem : public System
{
public:
	EntitySystem();
	~EntitySystem();

	void update() override;

	void addEntity(std::type_index*);
	void createChunk(EntityArchetype*);
	void createChunkArchetype(EntityArchetype*);
	ChunkArchetypeElement* findChunkArchetype(EntityArchetype*);

private:
	ChunkArchetypeElement* chunkArchetypeList;
	ChunkArchetypeElement* chunkArchetypeListLast;
};