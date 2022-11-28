#pragma once

#include <system.h>
#include <forward_list>
#include <typeindex>
#include <entityArchetype.h>
#include <chunkArchetypeElement.h>
#include <component.h>
#include <entity.h>
#include <mutex>

// Each chunk is 16kB
const unsigned short chunkSize = 16384;

class EntityManager
{
private:
	static EntityManager* instance;
	static std::mutex mutex;

protected:
	EntityManager();

public:
	~EntityManager();

	// Signleton
	EntityManager(EntityManager& other);
	void operator=(const EntityManager&) = delete;
	static EntityManager& GetInstance();

	int nextEntityIndex;

	void update();

	template <class T>
	T& getComponent(Entity&);

	Entity addEntity(EntityArchetype&);
	Chunk* createChunk(EntityArchetype&);
	Chunk* createChunk(ChunkArchetypeElement&);
	ChunkArchetypeElement* createChunkArchetype(EntityArchetype&);
	ChunkArchetypeElement* findChunkArchetype(EntityArchetype&);
	std::forward_list<ChunkArchetypeElement*>* findChunkArchetypesWithComponent(Component&);

private:
	ChunkArchetypeElement* chunkArchetypeList;
};

template <class T>
inline T& EntityManager::getComponent(Entity& entity)
{
	// Get pointer to component stream
	char* componentStream = (char*)(entity.chunk + 1);
	T* tComponentStream = (T*)(componentStream + entity.archetype->getComponentOffset(&Component().init<T>()));
	T& ref = *(tComponentStream + entity.index);
	return ref;
}