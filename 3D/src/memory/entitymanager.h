#pragma once
#include "chunkarchetypeelement.h"
#include "entity.h"

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
	T* GetComponentP(const Entity&) const;
	template <class T>
	T& GetComponent(const Entity&) const;

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
inline T* EntityManager::GetComponentP(const Entity& entity) const
{
	// Get pointer to component stream
	char* pComponentStream = (char*)(entity.pChunk + 1);
	short offset = entity.pArchetype->GetComponentOffset(Component().Init<T>());

	if (offset < 0)
		return nullptr;

	T* pTComponentStream = (T*)(pComponentStream + static_cast<uint64_t>(entity.pChunk->maxEntities) * offset);
	T* p = (pTComponentStream + entity.index);
	return p;
}

template <class T>
inline T& EntityManager::GetComponent(const Entity& entity) const
{
	// Get pointer to component stream
	char* pComponentStream = (char*)(entity.pChunk + 1);
	short offset = entity.pArchetype->GetComponentOffset(Component().Init<T>());
	T* pTComponentStream = (T*)(pComponentStream + static_cast<uint64_t>(entity.pChunk->maxEntities) * offset);
	T& r = *(pTComponentStream + entity.index);
	return r;
}