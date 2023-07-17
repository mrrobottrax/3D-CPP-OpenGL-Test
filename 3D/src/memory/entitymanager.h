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

struct TableEntry
{
	TableEntry(const EntityPointer& p)
		: p(p), version(0)
	{};

	EntityPointer p;
	gEntityIndex_t version;
};

class EntityManager
{
public:
	EntityManager() : entityIndices(), entityTable(), numberOfEntities(),
		chunkArchetypeList()
	{};

	~EntityManager()
	{};

	gEntityIndex_t numberOfEntities;
	std::vector<gEntityIndex_t> entityIndices;
	std::vector<TableEntry> entityTable;

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

	inline const bool EntityExists(const Entity& entity) const
	{
		return (entity.index < entityTable.size()
			&& entity.version == entityTable[entity.index].version);
	};

	inline const EntityPointer GetEntityPointer(const Entity& entity) const
	{
		assert(("Tried to get pointer to non-existant entity", EntityExists(entity)));
		return entityTable[entity.index].p;
	};

	void DeleteEntity(const Entity&);
	void DeleteChunkArchetype(ChunkArchetypeElement*);
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

	T* pTComponentStream = (T*)(pComponentStream + static_cast<size_t>(p.pChunk->pChunkArchetype->maxEntities) * offset);
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

	T* pTComponentStream = (T*)(pComponentStream + static_cast<size_t>(p.pChunk->pChunkArchetype->maxEntities) * offset);
	T& r = *(pTComponentStream + p.indexInChunk);
	return r;
}

template <class T>
inline T* EntityManager::GetComponentP(const Entity& entity) const
{
	const EntityPointer p = entityManager.GetEntityPointer(entity);

	// Get pointer to component stream
	char* pComponentStream = (char*)(p.pChunk + 1);
	int offset = p.pChunk->pChunkArchetype->archetype.GetComponentOffset(Component().Init<T>());

	if (offset < 0)
		return nullptr;

	T* pTComponentStream = (T*)(pComponentStream + static_cast<size_t>(p.pChunk->pChunkArchetype->maxEntities) * offset);
	T* pT = (pTComponentStream + p.indexInChunk);
	return pT;
}

template <class T>
inline T& EntityManager::GetComponent(const Entity& entity) const
{
	const EntityPointer p = entityManager.GetEntityPointer(entity);

	// Get pointer to component stream
	char* pComponentStream = (char*)(p.pChunk + 1);
	int offset = p.pChunk->pChunkArchetype->archetype.GetComponentOffset(Component().Init<T>());

	assert(("Tried to get reference to component not attached to entity!", offset >= 0));

	T* pTComponentStream = (T*)(pComponentStream + static_cast<size_t>(p.pChunk->pChunkArchetype->maxEntities) * offset);
	T& r = *(pTComponentStream + p.indexInChunk);
	return r;
}