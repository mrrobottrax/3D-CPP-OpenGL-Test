#include <entitySystem.h>
#include <idComponent.h>
#include <positionComponent.h>
#include <cameraComponent.h>
#include <iostream>

EntitySystem::EntitySystem()
{
	chunkArchetypeList = nullptr;
	chunkArchetypeListLast = nullptr;

	Archetype player{};

	std::type_index components[] = { typeid(IdComponent), typeid(PositionComponent), typeid(CameraComponent)};

	player.componentCount = 3;
	player.components = components;
	createChunkArchetype(&player);
}

EntitySystem::~EntitySystem()
{
	// Empty list
	if (chunkArchetypeList == nullptr)
		return;

	ChunkArchetypeElement* nextElement = chunkArchetypeList->next;
	while (true)
	{
		delete[] chunkArchetypeList;

		if (nextElement == nullptr)
		{
			break;
		}

		chunkArchetypeList = nextElement;
		nextElement = chunkArchetypeList->next;
	}
}

void EntitySystem::addEntity(std::type_index* archetype)
{

}

void EntitySystem::createChunkArchetype(Archetype* archetype)
{
	unsigned short sizeOfChunkArchetype = sizeof(ChunkArchetypeElement);

	// Allocate space for archetype
	unsigned short arraySize = archetype->componentCount * sizeof(std::type_index);

	if (arraySize <= 0)
	{
		std::cout << "Tried to create a chunk archetype with no components!";
		return;
	}

	unsigned short size = sizeOfChunkArchetype + arraySize;
	ChunkArchetypeElement* element = (ChunkArchetypeElement*)(malloc(size));

	if (element == nullptr)
		return;

	element->next = nullptr;
	element->firstChunk = nullptr;

	// Get memory location of the archetype components array
	std::type_index* componentsArray = (std::type_index*)(element + 1);

	memcpy(componentsArray, archetype->components, arraySize);

	element->archetype.components = componentsArray;
	element->archetype.componentCount = archetype->componentCount;

	// First element in list
	if (chunkArchetypeList == nullptr)
	{
		chunkArchetypeList = element;
		chunkArchetypeListLast = element;
		return;
	}

	// Get last element
	chunkArchetypeListLast->next = element;
	chunkArchetypeListLast = element;
}

void EntitySystem::createChunk(Archetype* archetype)
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

ChunkArchetypeElement* EntitySystem::findChunkArchetype(Archetype* archetype)
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
			if (chunkArchetype->archetype == *archetype)
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