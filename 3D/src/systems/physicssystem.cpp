#include <pch.h>
#include <systems/physicssystem.h>
#include <systems/systemmanager.h>
#include <debugtools/debugdraw.h>

#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/idcomponent.h>
#include <components/hullcollider.h>

#include <common/math.h>

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

	std::vector<CollisionPair> pairs;

	std::vector<ChunkArchetypeElement*>* archetypes = em.FindChunkArchetypesWithComponent(Component().init<RigidBodyComponent>());

	if (archetypes == nullptr)
		return;

	// For each archetype
	for (std::vector<ChunkArchetypeElement*>::iterator chunkArchetypeIt = archetypes->begin(); chunkArchetypeIt != archetypes->end(); ++chunkArchetypeIt)
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

				std::vector<ChunkArchetypeElement*>::iterator chunkArchetypeIt2 = chunkArchetypeIt;
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
								pairs.push_back(pair);
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
	for (auto pairsIt = pairs.begin(); pairsIt != pairs.end(); ++pairsIt)
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
		//std::cout << "\n";
#endif // DEBUG
	}
}

bool IsSeperatingPlane(gMath::Plane testPlane, ConvexHull* hull, glm::vec3 offset, glm::fquat rotation)
{
	/*for (int v = 0; v < hull->vertCount; ++v)
	{
		int i = v * 3;

		glm::vec3 point;
		point.x = hull->vertices[i];
		point.y = hull->vertices[i + 1];
		point.z = hull->vertices[i + 2];

		point = rotation * point - offset;

		float dot = glm::dot(point, testPlane.normal) - testPlane.dist;

		if (dot <= 0)
		{
			return false;
		}
	}*/

	return true;
}

bool SatFaceTest(HullCollider hullA, PositionComponent positionA, RotationComponent rotationA,
	HullCollider hullB, PositionComponent positionB, RotationComponent rotationB)
{
//	glm::vec3 offset = positionA.value - positionB.value;
//
//	Plane testPlane;
//
//	for (int f = 0; f < hullA.hull->planeCount; ++f)
//	{
//		testPlane = hullA.hull->planes[f];
//		testPlane.normal = rotationA.value * testPlane.normal;
//
//		if (IsSeperatingPlane(testPlane, hullB.hull, offset, rotationB.value))
//			return true;
//	}
//
	return false;
}

bool SatEdgeTest(HullCollider hullA, PositionComponent positionA, RotationComponent rotationA,
	HullCollider hullB, PositionComponent positionB, RotationComponent rotationB)
{
//	// Check all edge combinations
//	//TODO: not all edge combinations are necessary to check
//	glm::vec3 offset = positionA.value - positionB.value;
//	for (int eA = 0; eA < hullA.hull->edgeCount; ++eA)
//	{
//		int eAi = eA * 6;
//
//		glm::vec3 edgeAHead;
//		edgeAHead.x = hullA.hull->edges[eAi];
//		edgeAHead.y = hullA.hull->edges[eAi + 1];
//		edgeAHead.z = hullA.hull->edges[eAi + 2];
//		edgeAHead = rotationA.value * edgeAHead;
//
//		glm::vec3 edgeADir;
//		edgeADir.x = hullA.hull->edges[eAi + 3];
//		edgeADir.y = hullA.hull->edges[eAi + 4];
//		edgeADir.z = hullA.hull->edges[eAi + 5];
//		edgeADir = rotationA.value * edgeADir;
//
//		for (int eB = 0; eB < hullB.hull->edgeCount; ++eB)
//		{
//			int eBi = eB * 6;
//
//			glm::vec3 edgeBHead;
//			edgeBHead.x = hullA.hull->edges[eBi];
//			edgeBHead.y = hullA.hull->edges[eBi + 1];
//			edgeBHead.z = hullA.hull->edges[eBi + 2];
//			edgeBHead = rotationB.value * edgeBHead;
//			edgeBHead -= offset;
//
//			glm::vec3 edgeBDir;
//			edgeBDir.x = hullA.hull->edges[eBi + 3];
//			edgeBDir.y = hullA.hull->edges[eBi + 4];
//			edgeBDir.z = hullA.hull->edges[eBi + 5];
//			edgeBDir = rotationB.value * edgeBDir;
//
//			Plane testPlane;
//			testPlane.normal = glm::normalize(glm::cross(edgeADir, edgeBDir));
//
//			// Check if the normal is facing the right direction
//			if (glm::dot(edgeAHead, testPlane.normal) < 0)
//			{
//				testPlane.normal = -testPlane.normal;
//			}
//
//			// Get point furthest in normal direction (support point)
//			//TODO: There is a way to do this without the support point
//			glm::vec3 support;
//			{
//				int bestI = 0;
//				float bestDot = -1;
//				for (int v = 0; v < hullA.hull->vertCount; ++v)
//				{
//					int i = v * 3;
//
//					glm::vec3 point;
//					point.x = hullA.hull->vertices[i];
//					point.y = hullA.hull->vertices[i + 1];
//					point.z = hullA.hull->vertices[i + 2];
//
//					float dot = glm::dot(point, testPlane.normal);
//					if (dot >= bestDot)
//					{
//						bestI = i;
//						bestDot = dot;
//					}
//				}
//				support.x = hullA.hull->vertices[bestI];
//				support.y = hullA.hull->vertices[bestI + 1];
//				support.z = hullA.hull->vertices[bestI + 2];
//			}
//
//			// Move plane to support point
//			testPlane.dist = glm::dot(support, testPlane.normal);
//
//			if (IsSeperatingPlane(testPlane, hullB.hull, offset, rotationB.value))
//				return true;
//		}
//	}

	return false;
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

	// Check faces of A
	if (SatFaceTest(hullA, positionA, rotationA, hullB, positionB, rotationB))
	{

#ifdef DEBUG
		std::cout << "Face test A\n";
#endif // DEBUG

		return;
	}

	// Check faces of B
	if (SatFaceTest(hullB, positionB, rotationB, hullA, positionA, rotationA))
	{

#ifdef DEBUG
		std::cout << "Face test B\n";
#endif // DEBUG

		return;
	}

	// Check edge combinations
	if (SatEdgeTest(hullA, positionA, rotationA, hullB, positionB, rotationB))
	{

#ifdef DEBUG
		std::cout << "Edge test\n";
#endif // DEBUG

		return;
	}
}