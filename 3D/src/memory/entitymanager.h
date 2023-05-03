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
	static EntityManager* pInstance;
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
	std::vector<ChunkArchetypeElement*>* FindChunkArchetypesWithComponent(Component&);

	void DeleteAllEntities();

private:
	ChunkArchetypeElement* chunkArchetypeList;
}; inline EntityManager entityManager;

template <class T>
inline T& EntityManager::GetComponent(Entity& entity)
{
	// Get pointer to component stream
	char* pComponentStream = (char*)(entity.pChunk + 1);
	T* pTComponentStream = (T*)(pComponentStream + static_cast<uint64_t>(entity.pChunk->maxEntities) * entity.pArchetype->getComponentOffset(Component().init<T>()));
	T& ref = *(pTComponentStream + entity.index);
	return ref;
}