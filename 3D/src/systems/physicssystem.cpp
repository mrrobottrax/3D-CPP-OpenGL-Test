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

FaceQuery SatFaceTest(HullCollider referenceHull, PositionComponent referencePosition, RotationComponent referenceRotation,
	HullCollider incidentHull, PositionComponent incidentPosition, RotationComponent incidentRotation)
{
	gMath::Plane testPlane;
	FaceQuery query = FaceQuery();
	query.seperation = -FLT_MAX;

	for (int f = 0; f < incidentHull.pHull->faceCount; ++f)
	{
		testPlane = incidentHull.pHull->faces[f].plane;

		// TODO: Should be a way to skip converting to world space first

		// World space
		testPlane.normal = incidentRotation.value * testPlane.normal;
		testPlane.dist = -glm::dot(-(incidentPosition.value + testPlane.normal * testPlane.dist), testPlane.normal);

		// Reference space
		testPlane.dist = -glm::dot(referencePosition.value - (testPlane.normal * testPlane.dist), testPlane.normal);
		testPlane.normal = glm::inverse(referenceRotation.value) * testPlane.normal;

		float seperation = GetSeperationDepth(testPlane, incidentHull.pHull);

		if (seperation > query.seperation)
		{
			query.seperation = seperation;
			query.pFace = &referenceHull.pHull->faces[f];
			query.plane = testPlane;
		}

		// TODO: Could cause problems?
		if (seperation > 0)
		{

#ifdef PHYS_DEBUG
			gMath::Plane drawPlane = query.plane;
			drawPlane.normal = referenceRotation.value * drawPlane.normal;

			debugDraw.DrawPlane(referencePosition.value, drawPlane, 1.5f, 1.5f, { 0, 1, 1 });
#endif // PHYS_DEBUG

			return query;
		}
	}

	return query;
}

EdgeQuery SatEdgeTest(HullCollider referenceHull, PositionComponent refrencePosition, RotationComponent referenceRotation,
	HullCollider incidentHull, PositionComponent incidentPosition, RotationComponent incidentRotation)
{
	EdgeQuery query;
	query.seperation = -FLT_MAX;

	// Check all edge combinations
	// TODO: not all edge combinations are necessary to check
	for (int edgeAIndex = 0; edgeAIndex < referenceHull.pHull->edgeCount; ++edgeAIndex)
	{
		glm::vec3 edgeATail = referenceHull.pHull->edges[edgeAIndex].pHalfA->pTail->position;
		glm::vec3 edgeADir = referenceHull.pHull->edges[edgeAIndex].pHalfB->pTail->position - edgeATail;

		// TODO: This should be able to be optimized
		edgeATail = referenceRotation.value * edgeATail + refrencePosition.value;
		edgeADir = referenceRotation.value * edgeADir;

		edgeATail = edgeATail - incidentPosition.value;
		edgeATail = glm::inverse(incidentRotation.value) * edgeATail;
		edgeADir = glm::inverse(incidentRotation.value) * edgeADir;

		for (int edgeBIndex = 0; edgeBIndex < incidentHull.pHull->edgeCount; ++edgeBIndex)
		{
			glm::vec3 edgeBTail = incidentHull.pHull->edges[edgeBIndex].pHalfA->pTail->position;
			glm::vec3 edgeBDir = incidentHull.pHull->edges[edgeBIndex].pHalfB->pTail->position - edgeBTail;

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
				for (int v = 0; v < incidentHull.pHull->vertCount; ++v)
				{
					glm::vec3 point = incidentHull.pHull->verts[v].position;

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
			testPlane.normal = incidentRotation.value * testPlane.normal;
			testPlane.dist = -glm::dot(-(incidentPosition.value + testPlane.normal * testPlane.dist), testPlane.normal);

			// HullA space
			testPlane.dist = -glm::dot(refrencePosition.value - (testPlane.normal * testPlane.dist), testPlane.normal);
			testPlane.normal = glm::inverse(referenceRotation.value) * testPlane.normal;

			float seperation = GetSeperationDepth(testPlane, referenceHull.pHull);

			if (seperation >= query.seperation)
			{
				query.seperation = seperation;
				query.pEdgeA = &referenceHull.pHull->edges[edgeAIndex];
				query.pEdgeB = &incidentHull.pHull->edges[edgeBIndex];
				query.plane = testPlane;
			}

			// TODO: Could cause problems?
			if (seperation > 0)
			{
#ifdef PHYS_DEBUG
				gMath::Plane drawPlane = query.plane;
				drawPlane.normal = referenceRotation.value * drawPlane.normal;
				debugDraw.DrawPlane(refrencePosition.value, drawPlane, 1.5f, 1.5f, { 1, 1, 0 });
#endif // PHYS_DEBUG

				return query;
			}
		}
	}

	return query;
}

void PhysicsSystem::HullVsHull(Entity referenceEntity, Entity incidentEntity)
{
	// Seperating Axis Theorum

	EntityManager& em = entityManager;

	PositionComponent& referencePosition = em.GetComponent<PositionComponent>(referenceEntity);
	PositionComponent& incidentPosition = em.GetComponent<PositionComponent>(incidentEntity);
	RotationComponent& referenceRotation = em.GetComponent<RotationComponent>(referenceEntity);
	RotationComponent& incidentRotation = em.GetComponent<RotationComponent>(incidentEntity);

	HullCollider& referenceHull = em.GetComponent<HullCollider>(referenceEntity);
	HullCollider& incidentHull = em.GetComponent<HullCollider>(incidentEntity);

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

	DrawHull(referenceHull.pHull, referencePosition.value, referenceRotation.value, {1, 0, 0});
	DrawHull(incidentHull.pHull, incidentPosition.value, incidentRotation.value, {0, 0, 1});

#endif // PHYS_DEBUG

	FaceQuery faceQueryA = SatFaceTest(referenceHull, referencePosition, referenceRotation, incidentHull, incidentPosition, incidentRotation);

	// Check faces of A
	if (faceQueryA.seperation > 0)
	{

#ifdef PHYS_DEBUG
		std::cout << "Face test A: " << faceQueryA.seperation << "\n";
#endif // PHYS_DEBUG

		return;
	}

	FaceQuery faceQueryB = SatFaceTest(incidentHull, incidentPosition, incidentRotation, referenceHull, referencePosition, referenceRotation);

	// Check faces of B
	if (faceQueryB.seperation > 0)
	{

#ifdef PHYS_DEBUG
		std::cout << "Face test B: " << faceQueryB.seperation << "\n";
#endif // PHYS_DEBUG

		return;
	}

	EdgeQuery edgeQuery = SatEdgeTest(referenceHull, referencePosition, referenceRotation, incidentHull, incidentPosition, incidentRotation);

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
			drawPlane.normal = incidentRotation.value * drawPlane.normal;

			debugDraw.DrawPlane(incidentPosition.value, drawPlane, 1.5f, 1.5f, { 0, 1, 1 });
#endif // PHYS_DEBUG
		}
		else
		{
			seperation = faceQueryB.seperation;

#ifdef PHYS_DEBUG
			gMath::Plane drawPlane = faceQueryB.plane;
			drawPlane.normal = incidentRotation.value * drawPlane.normal;

			debugDraw.DrawPlane(incidentPosition.value, drawPlane, 1.5f, 1.5f, { 0, 1, 1 });
#endif // PHYS_DEBUG
		}
	}
	else
	{
		seperation = edgeQuery.seperation;

#ifdef PHYS_DEBUG
		gMath::Plane drawPlane = edgeQuery.plane;
		drawPlane.normal = referenceRotation.value * drawPlane.normal;
		debugDraw.DrawPlane(referencePosition.value, drawPlane, 1.5f, 1.5f, { 1, 1, 0 });
#endif // PHYS_DEBUG
	}

#ifdef PHYS_DEBUG
	std::cout << "Collide: " << seperation << "\n";
#endif // PHYS_DEBUG

}