#pragma once
#include "chunkarchetypeelement.h"
#include "entity.h"
#include <common/types.h>

// Each chunk is 16kB
constexpr gSize_t chunkSize = 16384;

typedef unsigned short gChunkIndex_t;

struct EntityPointer
{
	EntityPointer(Chunk* pChunk, const gEntityIndex_t& indexInChunk)
		: pChunk(pChunk), indexInChunk(indexInChunk)
	{};

	Chunk* pChunk;
	gEntityIndex_t indexInChunk;
};

class EntityManager
{
private:
	static std::mutex mutex;

public:
	EntityManager() : entityIndices(), entityPointers(), numberOfEntities(),
		chunkArchetypeList()
	{};

	~EntityManager()
	{};

	gEntityIndex_t numberOfEntities;
	std::vector<gEntityIndex_t> entityIndices;
	std::vector<EntityPointer> entityPointers;

	template <class T>
	T* GetComponentP(const EntityPointer&) const;
	template <class T>
	T& GetComponent(const EntityPointer&) const;

	template <class T>
	T* GetComponentP(const Entity&) const;
	template <class T>
	T& GetComponent(const Entity&) const;

	Entity AddEntity(EntityArchetype&);
	Chunk* CreateChunk(EntityArchetype&);
	Chunk* CreateChunk(ChunkArchetypeElement&);
	ChunkArchetypeElement* CreateChunkArchetype(EntityArchetype&);
	ChunkArchetypeElement* FindChunkArchetype(EntityArchetype&);
	std::vector<ChunkArchetypeElement*> FindChunkArchetypesWithComponent(Component&);

	inline const EntityPointer& GetPointerAtIndex(gEntityIndex_t index) const
	{
		assert(("Tried to get pointer to non-existant entity", index < entityPointers.size()));
		return entityPointers[index];
	};

	void DeleteAllEntities();

private:
	ChunkArchetypeElement* chunkArchetypeList;
}; inline EntityManager entityManager;

template <class T>
inline T* EntityManager::GetComponentP(const EntityPointer& p) const
{
	// Get pointer to component stream
	char* pComponentStream = (char*)(p.pChunk + 1);
	int offset = p.pChunk->pChunkArchetype->archetype.GetComponentOffset(Component().Init<T>());

	if (offset < 0)
		return nullptr;

	T* pTComponentStream = (T*)(pComponentStream + static_cast<size_t>(p.pChunk->maxEntities) * offset);
	T* pT = (pTComponentStream + p.indexInChunk);
	return pT;
}

template <class T>
inline T& EntityManager::GetComponent(const EntityPointer& p) const
{
	// Get pointer to component stream
	char* pComponentStream = (char*)(p.pChunk + 1);
	int offset = p.pChunk->pChunkArchetype->archetype.GetComponentOffset(Component().Init<T>());

	assert(("Tried to get reference to component not attached to entity!", offset >= 0));

	T* pTComponentStream = (T*)(pComponentStream + static_cast<size_t>(p.pChunk->maxEntities) * offset);
	T& r = *(pTComponentStream + p.indexInChunk);
	return r;
}

template <class T>
inline T* EntityManager::GetComponentP(const Entity& entity) const
{
	const EntityPointer& p = entityManager.GetPointerAtIndex(entity.index);

	// Get pointer to component stream
	char* pComponentStream = (char*)(p.pChunk + 1);
	int offset = p.pChunk->pChunkArchetype->archetype.GetComponentOffset(Component().Init<T>());

	if (offset < 0)
		return nullptr;

	T* pTComponentStream = (T*)(pComponentStream + static_cast<size_t>(p.pChunk->maxEntities) * offset);
	T* pT = (pTComponentStream + p.indexInChunk);
	return pT;
}

template <class T>
inline T& EntityManager::GetComponent(const Entity& entity) const
{
	const EntityPointer& p = entityManager.GetPointerAtIndex(entity.index);

	// Get pointer to component stream
	char* pComponentStream = (char*)(p.pChunk + 1);
	int offset = p.pChunk->pChunkArchetype->archetype.GetComponentOffset(Component().Init<T>());

	assert(("Tried to get reference to component not attached to entity!", offset >= 0));

	T* pTComponentStream = (T*)(pComponentStream + static_cast<size_t>(p.pChunk->maxEntities) * offset);
	T& r = *(pTComponentStream + p.indexInChunk);
	return r;
}