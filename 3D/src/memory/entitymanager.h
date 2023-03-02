#pragma once

#include <memory/entityArchetype.h>
#include <memory/chunkArchetypeElement.h>
#include <memory/component.h>
#include <memory/entity.h>

// Each chunk is 16kB
const unsigned short chunkSize = 16384;

class EntityManager
{
private:
	static EntityManager* instance;
	static std::mutex mutex;

public:
	EntityManager();
	~EntityManager();

	int nextEntityIndex;

	template <class T>
	T& GetComponent(Entity&);

	Entity AddEntity(EntityArchetype&);
	Chunk* CreateChunk(EntityArchetype&);
	Chunk* CreateChunk(ChunkArchetypeElement&);
	ChunkArchetypeElement* CreateChunkArchetype(EntityArchetype&);
	ChunkArchetypeElement* FindChunkArchetype(EntityArchetype&);
	std::forward_list<ChunkArchetypeElement*>* FindChunkArchetypesWithComponent(Component&);

private:
	ChunkArchetypeElement* chunkArchetypeList;
}; inline EntityManager entityManager;

template <class T>
inline T& EntityManager::GetComponent(Entity& entity)
{
	// Get pointer to component stream
	char* componentStream = (char*)(entity.chunk + 1);
	T* tComponentStream = (T*)(componentStream + static_cast<uint64_t>(entity.chunk->maxEntities) * entity.archetype->getComponentOffset(Component().init<T>()));
	T& ref = *(tComponentStream + entity.index);
	return ref;
}