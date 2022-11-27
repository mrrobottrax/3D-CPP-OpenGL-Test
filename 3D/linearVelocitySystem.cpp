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
	Component linearVelocityComponent = Component(typeid(LinearVelocityComponent), sizeof(LinearVelocityComponent));
	Component positionComponent = Component(typeid(PositionComponent), sizeof(PositionComponent));

	EntityManager* em = (EntityManager*)systemManager::getSystem(typeid(EntityManager).hash_code());
	std::forward_list<ChunkArchetypeElement*>* archetypes = em->findChunkArchetypesWithComponent(&linearVelocityComponent);

	// For each archetype
	for (std::forward_list<ChunkArchetypeElement*>::iterator chunkArchetypeIt = archetypes->begin(); chunkArchetypeIt != archetypes->end(); ++chunkArchetypeIt)
	{
		// For each chunk
		for (Chunk* chunk = (*chunkArchetypeIt)->firstChunk; chunk != nullptr; chunk = chunk->next)
		{
			// Get pointer to component stream
			char* componentStream = (char*)(chunk + 1);
			PositionComponent* positionComponentStream = (PositionComponent*)(componentStream + ((*chunkArchetypeIt)->archetype.getComponentOffset(&positionComponent)));
			LinearVelocityComponent* linearVelocityComponentStream = (LinearVelocityComponent*)(componentStream + ((*chunkArchetypeIt)->archetype.getComponentOffset(&linearVelocityComponent)));

			// For each entity
			for (int i = 0; i < chunk->numberOfEntities; i++)
			{
				// Get position of component in component stream
				PositionComponent* positionComponent = positionComponentStream + i;
				LinearVelocityComponent* velocityComponent = linearVelocityComponentStream + i;
				positionComponent->x += velocityComponent->x;
				positionComponent->y += velocityComponent->y;
				positionComponent->z += velocityComponent->z;
			}
		}
	}

	delete archetypes;
}
