#pragma once

#include <system.h>
#include <forward_list>
#include <typeindex>
#include <entityArchetype.h>
#include <chunkArchetypeElement.h>
#include <component.h>

// Each chunk is 16kB
const unsigned short chunkSize = 16384;

class EntityManager : public System
{
public:
	EntityManager();
	~EntityManager();

	void update() override;

	void addEntity(EntityArchetype*);
	Chunk* createChunk(EntityArchetype*);
	Chunk* createChunk(ChunkArchetypeElement*);
	ChunkArchetypeElement* createChunkArchetype(EntityArchetype*);
	ChunkArchetypeElement* findChunkArchetype(EntityArchetype*);
	std::forward_list<ChunkArchetypeElement*>* findChunkArchetypesWithComponent(Component*);

private:
	ChunkArchetypeElement* chunkArchetypeList;
	ChunkArchetypeElement* chunkArchetypeListLast;
};