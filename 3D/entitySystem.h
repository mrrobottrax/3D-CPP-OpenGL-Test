#pragma once

#include <system.h>
#include <forward_list>
#include <typeindex>
#include <entityArchetype.h>
#include <chunkArchetypeElement.h>

// Each chunk is 16kB
const unsigned short chunkSize = 16384;

class EntitySystem : public System
{
public:
	EntitySystem();
	~EntitySystem();

	void update() override;

	void addEntity(EntityArchetype*);
	Chunk* createChunk(EntityArchetype*);
	Chunk* createChunk(ChunkArchetypeElement*);
	ChunkArchetypeElement* createChunkArchetype(EntityArchetype*);
	ChunkArchetypeElement* findChunkArchetype(EntityArchetype*);

private:
	ChunkArchetypeElement* chunkArchetypeList;
	ChunkArchetypeElement* chunkArchetypeListLast;
};