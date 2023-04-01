#include <pch.h>
#include <systems/physicssystem.h>
#include <systems/systemmanager.h>
#include <debugtools/debugdraw.h>

#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/idcomponent.h>
#include <components/hullcollider.h>

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

		switch (rbA.colliderType)
		{
		case Hull:
			switch (rbB.colliderType)
			{
			case Hull:
				// HULL VS HULL
				HullVsHull(entityA, entityB);
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}

#ifdef DEBUG
		std::cout << "\n";
#endif // DEBUG
	}
}

void PhysicsSystem::HullVsHull(Entity entityA, Entity entityB)
{
	// Seperating Axis Theorum

	EntityManager& em = entityManager;

	PositionComponent& positionA = em.GetComponent<PositionComponent>(entityA);
	PositionComponent& positionB = em.GetComponent<PositionComponent>(entityB);
	RotationComponent& rotationA = em.GetComponent<RotationComponent>(entityA);
	RotationComponent& rotationB = em.GetComponent<RotationComponent>(entityB);

	HullCollider& hullA = em.GetComponent<HullCollider>(entityA);
	HullCollider& hullB = em.GetComponent<HullCollider>(entityB);

	glm::vec3 offset = positionA.value - positionB.value;
	Plane testPlane;

	bool foundPlane = false;

	// Check faces of A
	for (int f = 0; f < hullA.hull->faceCount; ++f)
	{
		testPlane = hullA.hull->faces[f];
		testPlane.normal = rotationA.value * testPlane.normal;

		bool seperatingPlane = true;
		for (int v = 0; v < hullB.hull->vertCount; v += 3)
		{
			glm::vec3 point;
			point.x = hullB.hull->vertices[v];
			point.y = hullB.hull->vertices[v + 1];
			point.z = hullB.hull->vertices[v + 2];

			point = rotationB.value * point - offset;

			float dot = glm::dot(point, testPlane.normal) - testPlane.dist;

			if (dot <= 0)
			{
#ifdef DEBUG
				debugDraw.DrawLine(point + positionA.value, glm::vec3(point.x, point.y + 0.1f, point.z) + positionA.value);
#endif // DEBUG

				seperatingPlane = false;
				break;
			}
		}

		foundPlane = seperatingPlane;
		if (foundPlane)
		{

#ifdef DEBUG
			debugDraw.DrawPlane(glm::vec3(0) + positionA.value, testPlane.normal, testPlane.dist, 1, 1);
#endif // DEBUG

			break;
		}
	}

	if (foundPlane)
	{

#ifdef DEBUG
		std::cout << "Test1\n";
#endif // DEBUG

		return;
	}

	// Check faces of B
	for (int f = 0; f < hullB.hull->faceCount; ++f)
	{
		testPlane = hullB.hull->faces[f];
		testPlane.normal = rotationB.value * testPlane.normal;

		bool seperatingPlane = true;

		for (int v = 0; v < hullA.hull->vertCount * 3; v += 3)
		{
			glm::vec3 point;
			point.x = hullA.hull->vertices[v];
			point.y = hullA.hull->vertices[v + 1];
			point.z = hullA.hull->vertices[v + 2];

			point = rotationA.value * point + offset;

			float dot = glm::dot(point, testPlane.normal) - testPlane.dist;

			if (dot <= 0)
			{
				seperatingPlane = false;
				break;
			}
		}

		foundPlane = seperatingPlane;
		if (foundPlane)
			break;
	}

#ifdef DEBUG
	if (foundPlane)
	{
		std::cout << "Test2\n";
		return;
	}
#endif // DEBUG

}