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
	void* pRigidBodyA;
	void* pRigidBodyB;

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
		for (Chunk* pChunk = (*chunkArchetypeIt)->pFirstChunk; pChunk != nullptr; pChunk = pChunk->pNext)
		{
			// For each entity
			for (unsigned int i = 0; i < pChunk->numberOfEntities; i++)
			{
				// Check broad phase collision with all entities after this one

				// Check if the next entity exists in this chunk
				bool chunkComplete = i + 1u >= pChunk->numberOfEntities;

				// Check if the next entity is in a different archetype
				bool archetypeComplete = chunkComplete ? !pChunk->pNext : false;

				std::vector<ChunkArchetypeElement*>::iterator chunkArchetypeIt2 = chunkArchetypeIt;
				Chunk* pChunk2;

				// Next entity is a different archetype
				if (archetypeComplete)
				{
					++chunkArchetypeIt2;

					if (chunkArchetypeIt2 == archetypes->end())
					{
						break;
					}

					pChunk2 = (*chunkArchetypeIt2)->pFirstChunk;
				}
				else
				{
					pChunk2 = chunkComplete ? pChunk->pNext : pChunk;
				}

				Entity entity((*chunkArchetypeIt)->archetype, *pChunk, i);
				RigidBodyComponent& rb = em.GetComponent<RigidBodyComponent>(entity);

				// For each archetype
				while (chunkArchetypeIt2 != archetypes->end())
				{
					// For each chunk
					while (pChunk2 != nullptr)
					{
						// For each entity
						for (unsigned short i2 = archetypeComplete || chunkComplete ? 0 : i + 1; i2 < pChunk2->numberOfEntities; i2++)
						{
							Entity entity2((*chunkArchetypeIt2)->archetype, *pChunk2, i2);
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

						pChunk2 = pChunk2->pNext;
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

		RigidBodyComponent& rbA = *(RigidBodyComponent*)(*pairsIt).pRigidBodyA;
		RigidBodyComponent& rbB = *(RigidBodyComponent*)(*pairsIt).pRigidBodyB;

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
	}
}

// Find vertex with most penetration
float GetSeperationDepth(gMath::Plane testPlane, ConvexHull* pHull)
{
	float minSeperation = FLT_MAX;

	for (int v = 0; v < pHull->vertCount; ++v)
	{
		glm::vec3 point = pHull->verts[v].position;

		float dot = glm::dot(point, testPlane.normal) - testPlane.dist;

		if (dot <= minSeperation)
		{
			minSeperation = dot;
		}
	}

	return minSeperation;
}

FaceQuery SatFaceTest(HullCollider hullA, PositionComponent positionA, RotationComponent rotationA,
	HullCollider hullB, PositionComponent positionB, RotationComponent rotationB)
{
	gMath::Plane testPlane;
	FaceQuery query = FaceQuery();
	query.seperation = -FLT_MAX;

	for (int f = 0; f < hullA.pHull->faceCount; ++f)
	{
		testPlane = hullA.pHull->faces[f].plane;

		// TODO: Should be a way to skip converting to world space first

		// World space
		testPlane.normal = rotationA.value * testPlane.normal;
		testPlane.dist = -glm::dot(-(positionA.value + testPlane.normal * testPlane.dist), testPlane.normal);

		// HullB space
		testPlane.dist = -glm::dot(positionB.value - (testPlane.normal * testPlane.dist), testPlane.normal);
		testPlane.normal = glm::inverse(rotationB.value) * testPlane.normal;

		float seperation = GetSeperationDepth(testPlane, hullB.pHull);

		if (seperation > query.seperation)
		{
			query.seperation = seperation;
			query.pFace = &hullA.pHull->faces[f];
			query.plane = testPlane;
		}

		// TODO: Could cause problems?
		if (seperation > 0)
		{

#ifdef PHYS_DEBUG
			gMath::Plane drawPlane = query.plane;
			drawPlane.normal = rotationB.value * drawPlane.normal;

			debugDraw.DrawPlane(positionB.value, drawPlane, 1.5f, 1.5f, { 0, 1, 1 });
#endif // PHYS_DEBUG

			return query;
		}
	}

	return query;
}

EdgeQuery SatEdgeTest(HullCollider hullA, PositionComponent positionA, RotationComponent rotationA,
	HullCollider hullB, PositionComponent positionB, RotationComponent rotationB)
{
	EdgeQuery query;
	query.seperation = -FLT_MAX;

	// Check all edge combinations
	// TODO: not all edge combinations are necessary to check
	for (int edgeAIndex = 0; edgeAIndex < hullA.pHull->edgeCount; ++edgeAIndex)
	{
		glm::vec3 edgeATail = hullA.pHull->edges[edgeAIndex].pHalfA->pTail->position;
		glm::vec3 edgeADir = hullA.pHull->edges[edgeAIndex].pHalfB->pTail->position - edgeATail;

		// TODO: This should be able to be optimized
		edgeATail = rotationA.value * edgeATail + positionA.value;
		edgeADir = rotationA.value * edgeADir;

		edgeATail = edgeATail - positionB.value;
		edgeATail = glm::inverse(rotationB.value) * edgeATail;
		edgeADir = glm::inverse(rotationB.value) * edgeADir;

		for (int edgeBIndex = 0; edgeBIndex < hullB.pHull->edgeCount; ++edgeBIndex)
		{
			glm::vec3 edgeBTail = hullB.pHull->edges[edgeBIndex].pHalfA->pTail->position;
			glm::vec3 edgeBDir = hullB.pHull->edges[edgeBIndex].pHalfB->pTail->position - edgeBTail;

			gMath::Plane testPlane;
			testPlane.normal = glm::normalize(glm::cross(edgeADir, edgeBDir));

			// Check if the normal is facing the right direction
			if (glm::dot(edgeBTail, testPlane.normal) < 0)
			{
				testPlane.normal = -testPlane.normal;
			}

			// Get point furthest in normal direction (support point)
			// TODO: There is a way to do this without the support point
			glm::vec3 support;
			{
				float bestDot = 0;
				for (int v = 0; v < hullB.pHull->vertCount; ++v)
				{
					glm::vec3 point = hullB.pHull->verts[v].position;

					float dot = glm::dot(point, testPlane.normal);
					if (dot >= bestDot)
					{
						support = point;
						bestDot = dot;
					}
				}
			}

			// Move plane to support point
			testPlane.dist = glm::dot(support, testPlane.normal);

			// Move plane to HullA space
			// TODO: Should be a way to skip converting to world space first

			// World space
			testPlane.normal = rotationB.value * testPlane.normal;
			testPlane.dist = -glm::dot(-(positionB.value + testPlane.normal * testPlane.dist), testPlane.normal);

			// HullA space
			testPlane.dist = -glm::dot(positionA.value - (testPlane.normal * testPlane.dist), testPlane.normal);
			testPlane.normal = glm::inverse(rotationA.value) * testPlane.normal;

			float seperation = GetSeperationDepth(testPlane, hullA.pHull);

			if (seperation >= query.seperation)
			{
				query.seperation = seperation;
				query.pEdgeA = &hullA.pHull->edges[edgeAIndex];
				query.pEdgeB = &hullB.pHull->edges[edgeBIndex];
				query.plane = testPlane;
			}

			// TODO: Could cause problems?
			if (seperation > 0)
			{
#ifdef PHYS_DEBUG
				gMath::Plane drawPlane = query.plane;
				drawPlane.normal = rotationA.value * drawPlane.normal;
				debugDraw.DrawPlane(positionA.value, drawPlane, 1.5f, 1.5f, { 1, 1, 0 });
#endif // PHYS_DEBUG

				return query;
			}
		}
	}

	return query;
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

#ifdef PHYS_DEBUG

	auto DrawHull = [](const ConvexHull* pHull, const glm::vec3 position,
		const glm::fquat rotation, const glm::vec3 color)
	{
		for (int i = 0; i < pHull->edgeCount; ++i)
		{
			const qhEdge& edge = pHull->edges[i];

			glm::vec3 start = rotation * edge.pHalfA->pTail->position + position;
			glm::vec3 end = rotation * edge.pHalfB->pTail->position + position;

			debugDraw.DrawLine(start, end, color);
		}
	};

	DrawHull(hullA.pHull, positionA.value, rotationA.value, {1, 0, 0});
	DrawHull(hullB.pHull, positionB.value, rotationB.value, {0, 0, 1});

#endif // PHYS_DEBUG

	FaceQuery faceQueryA = SatFaceTest(hullA, positionA, rotationA, hullB, positionB, rotationB);

	// Check faces of A
	if (faceQueryA.seperation > 0)
	{

#ifdef PHYS_DEBUG
		std::cout << "Face test A: " << faceQueryA.seperation << "\n";
#endif // PHYS_DEBUG

		return;
	}

	FaceQuery faceQueryB = SatFaceTest(hullB, positionB, rotationB, hullA, positionA, rotationA);

	// Check faces of B
	if (faceQueryB.seperation > 0)
	{

#ifdef PHYS_DEBUG
		std::cout << "Face test B: " << faceQueryB.seperation << "\n";
#endif // PHYS_DEBUG

		return;
	}

	EdgeQuery edgeQuery = SatEdgeTest(hullA, positionA, rotationA, hullB, positionB, rotationB);

	// Check edge combinations
	if (edgeQuery.seperation > 0)
	{

#ifdef PHYS_DEBUG
		std::cout << "Edge test: " << edgeQuery.seperation << "\n";
#endif // PHYS_DEBUG

		return;
	}

	bool isFaceContactA = faceQueryA.seperation > edgeQuery.seperation;
	bool isFaceContactB = faceQueryB.seperation > edgeQuery.seperation;

	bool isFaceContact = isFaceContactA || isFaceContactB;

	float seperation;

	if (isFaceContact)
	{
		if (isFaceContactA)
		{
			seperation = faceQueryA.seperation;

#ifdef PHYS_DEBUG
			gMath::Plane drawPlane = faceQueryA.plane;
			drawPlane.normal = rotationB.value * drawPlane.normal;

			debugDraw.DrawPlane(positionB.value, drawPlane, 1.5f, 1.5f, { 0, 1, 1 });
#endif // PHYS_DEBUG
		}
		else
		{
			seperation = faceQueryB.seperation;

#ifdef PHYS_DEBUG
			gMath::Plane drawPlane = faceQueryB.plane;
			drawPlane.normal = rotationB.value * drawPlane.normal;

			debugDraw.DrawPlane(positionB.value, drawPlane, 1.5f, 1.5f, { 0, 1, 1 });
#endif // PHYS_DEBUG
		}
	}
	else
	{
		seperation = edgeQuery.seperation;

#ifdef PHYS_DEBUG
		gMath::Plane drawPlane = edgeQuery.plane;
		drawPlane.normal = rotationA.value * drawPlane.normal;
		debugDraw.DrawPlane(positionA.value, drawPlane, 1.5f, 1.5f, { 1, 1, 0 });
#endif // PHYS_DEBUG
	}

#ifdef PHYS_DEBUG
	std::cout << "Collide: " << seperation << "\n";
#endif // PHYS_DEBUG

}