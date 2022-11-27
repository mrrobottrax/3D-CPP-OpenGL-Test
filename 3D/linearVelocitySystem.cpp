#include <linearVelocitySystem.h>

#include <systemManager.h>
#include <entityManager.h>
#include <positionComponent.h>
#include <linearVelocityComponent.h>
#include <forward_list>

LinearVelocitySystem::LinearVelocitySystem()
{
}

LinearVelocitySystem::~LinearVelocitySystem()
{
}

void LinearVelocitySystem::update()
{
	EntityManager* em = (EntityManager*)systemManager::getSystem(typeid(EntityManager).hash_code());
	std::forward_list<ChunkArchetypeElement*>* archetypes = em->findChunkArchetypesWithComponent(&Component(typeid(PositionComponent), sizeof(PositionComponent)));

	// For each archetype
	for (std::forward_list<ChunkArchetypeElement*>::iterator it = archetypes->begin(); it != archetypes->end(); ++it)
	{
		// For each chunk
		for (Chunk* chunk = (*it)->firstChunk; chunk->next != nullptr; chunk = chunk->next)
		{
			// For each entity
			for (int i = 0; i < chunk->numberOfEntities; i++)
			{

			}
		}
	}

	delete archetypes;
}
