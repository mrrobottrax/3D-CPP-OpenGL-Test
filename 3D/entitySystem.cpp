#include <entitySystem.h>
#include <idComponent.h>
#include <positionComponent.h>
#include <cameraComponent.h>

EntitySystem::EntitySystem()
{
	chunkArchetypeList = nullptr;

	Archetype a;

	std::type_index components[] = {typeid(IdComponent), typeid(PositionComponent), typeid(CameraComponent)};

	a.componentCount = 3;
	a.components = components;
	createChunkArchetype(&a);
}

EntitySystem::~EntitySystem()
{
	delete[] chunkArchetypeList;
}

void EntitySystem::addEntity(std::type_index* archetype)
{

}

void EntitySystem::createChunkArchetype(Archetype* archetype)
{
	// Allocate space for archetype
	ChunkArchetypeElement* element = (ChunkArchetypeElement*)(malloc(sizeof(archetype) + archetype->componentCount * sizeof(std::type_index)));

	// First element in list
	if (chunkArchetypeList == nullptr)
	{
		chunkArchetypeList = element;
	}
}

void EntitySystem::createChunk(std::type_index* archetype)
{
	// Check if this archetype is already in chunkArchetypeList
	if (findChunkArchetype(archetype))
	{

	}
	else
	{
		// Add archetype to list
		//createChunkArchetype(archetype);
	}

	ChunkArchetypeElement* chunk = (ChunkArchetypeElement*)(malloc(sizeof(ChunkArchetypeElement)));
}

ChunkArchetypeElement* EntitySystem::findChunkArchetype(std::type_index* archetype)
{
	// Return null if list is empty
	if (chunkArchetypeList == nullptr)
	{
		return nullptr;
	}

	// Scan through entire linked list
	bool foundArchetype = false;
	ChunkArchetypeElement* chunkArchetype = chunkArchetypeList;
	while (true)
	{
		if (chunkArchetype->next != nullptr)
		{
			if (chunkArchetype->archetype == archetype)
			{
				foundArchetype = true;
				break;
			}
			chunkArchetype = chunkArchetype->next;
		}
		else
		{
			// End of list
			break;
		}
	}

	if (foundArchetype)
	{
		return chunkArchetype;
	}

	return nullptr;
}

void EntitySystem::update()
{
}