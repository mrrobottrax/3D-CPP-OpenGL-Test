#include <entitySystem.h>
#include <idComponent.h>
#include <positionComponent.h>
#include <cameraComponent.h>
#include <iostream>

EntitySystem::EntitySystem()
{
	chunkArchetypeList = nullptr;
	chunkArchetypeListLast = nullptr;

	// Create player
	std::type_index components[] = { typeid(IdComponent), typeid(PositionComponent), typeid(CameraComponent)};

	EntityArchetype player(components, 3);
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
		for (int i = 0; i < chunkArchetypeList->archetype.componentCount; i++)
		{
			std::cout << (i != 0 ? ", " : "") << (chunkArchetypeList->archetype.components[i]).name();
		}
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

void EntitySystem::createChunkArchetype(EntityArchetype* archetype)
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
	element->archetype = *archetype;

	// Get memory location of the archetype components array
	std::type_index* componentsArray = (std::type_index*)(element + 1);
	memcpy(componentsArray, archetype->components, arraySize);

	element->archetype.components = componentsArray;

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

void EntitySystem::createChunk(EntityArchetype* archetype)
{
	// Check if this archetype is already in chunkArchetypeList
	if (findChunkArchetype(archetype))
	{

	}
	else
	{
		// Add archetype to list
		createChunkArchetype(archetype);
	}

	ChunkArchetypeElement* chunk = (ChunkArchetypeElement*)(malloc(sizeof(ChunkArchetypeElement)));
}

ChunkArchetypeElement* EntitySystem::findChunkArchetype(EntityArchetype* archetype)
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