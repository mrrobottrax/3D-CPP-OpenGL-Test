#include <pch.h>
#include <systems/physicssystem.h>

#include <components/positioncomponent.h>

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
}

struct CollisionPair
{
	void* rigidBodyA;
	void* rigidBodyB;

	Entity entityA;
	Entity entityB;
};

void PhysicsSystem::Update()
{
	EntityManager& em = entityManager;

	// Broad phase ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	std::forward_list<CollisionPair> pairs;

	std::forward_list<ChunkArchetypeElement*>* archetypes = em.FindChunkArchetypesWithComponent(Component().init<RigidBodyComponent>());

	if (archetypes == nullptr)
		return;

	// For each archetype
	for (std::forward_list<ChunkArchetypeElement*>::iterator chunkArchetypeIt = archetypes->begin(); chunkArchetypeIt != archetypes->end(); ++chunkArchetypeIt)
	{
		// For each chunk
		for (Chunk* chunk = (*chunkArchetypeIt)->firstChunk; chunk != nullptr; chunk = chunk->next)
		{
			// For each entity
			for (unsigned short i = 0; i < chunk->numberOfEntities; i++)
			{
				Entity entity((*chunkArchetypeIt)->archetype, *chunk, i);
				RigidBodyComponent& rb = em.GetComponent<RigidBodyComponent>(entity);

				// Check broad phase collision with all entities after this one
				// For each archetype
				// Check if the next entity in the chunk exists
				bool chunkComplete = i + 1 >= chunk->numberOfEntities;
				bool archetypeComplete = chunkComplete ? chunk->next : false;
				for (std::forward_list<ChunkArchetypeElement*>::iterator chunkArchetypeIt2 = chunkArchetypeIt; chunkArchetypeIt2 != archetypes->end(); ++chunkArchetypeIt2)
				{
					// For each chunk
					for (Chunk* chunk2 = archetypeComplete ? chunk->next : chunk; chunk2 != nullptr; chunk2 = chunk2->next)
					{
						// For each entity
						for (unsigned short i2 = chunkComplete ? 0 : i; i2 < chunk2->numberOfEntities; i2++)
						{
							Entity entity2((*chunkArchetypeIt2)->archetype, *chunk2, i2);
							RigidBodyComponent& rb2 = em.GetComponent<RigidBodyComponent>(entity2);

							// No need to check for collisions when both are static
							if (!rb.isStatic || !rb2.isStatic)
							{
								CollisionPair pair = {
									&rb,
									&rb2,

									entity,
									entity2
								};
								pairs.push_front(pair);
							}
						}
					}
				}
			}
		}
	}

	delete archetypes;

	// Narrow phase  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// For each collision pair
	for (std::forward_list<CollisionPair>::iterator pairsIt = pairs.begin(); pairsIt != pairs.end(); ++pairsIt)
	{
		Entity entityA = (*pairsIt).entityA;
		Entity entityB = (*pairsIt).entityB;

		RigidBodyComponent& rbA = *(RigidBodyComponent*)(*pairsIt).rigidBodyA;
		RigidBodyComponent& rbB = *(RigidBodyComponent*)(*pairsIt).rigidBodyB;

		PositionComponent& positionA = em.GetComponent<PositionComponent>(entityA);
		PositionComponent& positionB = em.GetComponent<PositionComponent>(entityB);

		glm::vec3 posDiff;

		posDiff = positionA.value - positionB.value;

		float sqrLength = 0;
		for (int i = 0; i < 3; ++i)
		{
			sqrLength += posDiff[i];
		}

		std::cout << sqrLength << "\n";
	}
}