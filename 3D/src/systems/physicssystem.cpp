#include <pch.h>
#include <systems/physicssystem.h>

#include <components/positioncomponent.h>
#include <components/idcomponent.h>

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
			for (unsigned int i = 0; i < chunk->numberOfEntities; i++)
			{
				// Check broad phase collision with all entities after this one

				// Check if the next entity exists in this chunk
				bool chunkComplete = i + 1u >= chunk->numberOfEntities;

				// Check if the next entity is in a different archetype
				bool archetypeComplete = chunkComplete ? !chunk->next : false;

				std::forward_list<ChunkArchetypeElement*>::iterator chunkArchetypeIt2 = chunkArchetypeIt;
				Chunk* chunk2;

				// Next entity is a different archetype
				if (archetypeComplete)
				{
					++chunkArchetypeIt2;

					if (chunkArchetypeIt2 == archetypes->end())
					{
						break;
					}

					chunk2 = (*chunkArchetypeIt2)->firstChunk;
				}
				else
				{
					chunk2 = chunkComplete ? chunk->next : chunk;
				}

				Entity entity((*chunkArchetypeIt)->archetype, *chunk, i);
				RigidBodyComponent& rb = em.GetComponent<RigidBodyComponent>(entity);

				// For each archetype
				while (chunkArchetypeIt2 != archetypes->end())
				{
					// For each chunk
					while (chunk2 != nullptr)
					{
						// For each entity
						for (unsigned short i2 = archetypeComplete || chunkComplete ? 0 : i + 1; i2 < chunk2->numberOfEntities; i2++)
						{
							Entity entity2((*chunkArchetypeIt2)->archetype, *chunk2, i2);
							RigidBodyComponent& rb2 = em.GetComponent<RigidBodyComponent>(entity2);

							IdComponent& id = em.GetComponent<IdComponent>(entity);
							IdComponent& id2 = em.GetComponent<IdComponent>(entity2);

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

						chunk2 = chunk2->next;
					}

					++chunkArchetypeIt2;
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

	std::cout << "\n";
}