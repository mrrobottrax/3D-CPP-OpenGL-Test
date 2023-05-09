#include <pch.h>
#include <systems/physicssystem.h>
#include <systems/systemmanager.h>
#include <debugtools/debugdraw.h>

#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/masscomponent.h>
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

void ApplyVelocityAtPoint(const glm::vec3& velocity, VelocityComponent& velocityComponent, const glm::vec3& centerOfMass)
{
	velocityComponent.linear += velocity;
}

void ResolveManifold(const Manifold& manifold, const Entity& entityA, const Entity& entityB, EntityManager& em)
{
	const PositionComponent& positionA = em.GetComponent<PositionComponent>(entityA);
	const PositionComponent& positionB = em.GetComponent<PositionComponent>(entityB);

	const MassComponent& massA = em.GetComponent<MassComponent>(entityA);
	const MassComponent& massB = em.GetComponent<MassComponent>(entityB);

	VelocityComponent& velocityA = em.GetComponent<VelocityComponent>(entityA);
	VelocityComponent& velocityB = em.GetComponent<VelocityComponent>(entityB);

	const float invMassMatA[6][6] = {
		{ massA.inv_mass, 0, 0, 0, 0, 0 },
		{ 0, massA.inv_mass, 0, 0, 0, 0 },
		{ 0, 0, massA.inv_mass, 0, 0, 0 },
		{ 0, 0, 0, massA.inv_mass, 0, 0 },
		{ 0, 0, 0, 0, massA.inv_mass, 0 },
		{ 0, 0, 0, 0, 0, massA.inv_mass },
	};

	const float invMassMatB[6][6] = {
		{ massB.inv_mass, 0, 0, 0, 0, 0 },
		{ 0, massB.inv_mass, 0, 0, 0, 0 },
		{ 0, 0, massB.inv_mass, 0, 0, 0 },
		{ 0, 0, 0, massB.inv_mass, 0, 0 },
		{ 0, 0, 0, 0, massB.inv_mass, 0 },
		{ 0, 0, 0, 0, 0, massB.inv_mass },
	};

	float velA[6] = { velocityA.linear.x, velocityA.linear.y, velocityA.linear.z,
		velocityA.angular.x, velocityA.angular.y, velocityA.angular.z };

	for (int i = 0; i < manifold.numContacts; ++i)
	{
		const ContactPoint& contact = manifold.contacts[i];
		const glm::vec3& normal = manifold.normal;

		// Calculate jacobian matrix
		const glm::vec3 offset = contact.position - positionA.value;
		glm::vec3 cross = glm::cross(offset, normal);
		float jacobian[6] = { normal.x, normal.y, normal.z, cross.x, cross.y, cross.z };

		float impulse[6];

		glm::vec3 force = manifold.normal * manifold.seperation;

		force *= timescale.value;

		ApplyVelocityAtPoint(force * massA.inv_mass, velocityA, positionA.value);
		ApplyVelocityAtPoint(-force * massB.inv_mass, velocityB, positionB.value);

#ifdef PHYS_DEBUG
		debugDraw.DrawLine(contact.position, contact.position + manifold.normal * 0.25f, { 1, 0, 1 });
#endif // PHYS_DEBUG

	}
}

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

	std::vector<Manifold> manifolds;

	// For each collision pair
	for (auto pairsIt = pairs.begin(); pairsIt != pairs.end(); ++pairsIt)
	{
		Entity entityA = (*pairsIt).entityA;
		Entity entityB = (*pairsIt).entityB;

		RigidBodyComponent& rbA = *(RigidBodyComponent*)(*pairsIt).pRigidBodyA;
		RigidBodyComponent& rbB = *(RigidBodyComponent*)(*pairsIt).pRigidBodyB;

		Manifold manifold;

		bool collision = false;

		switch (rbA.colliderType)
		{
		case Hull:
			switch (rbB.colliderType)
			{
			case Hull:
				// HULL VS HULL
				collision = HullVsHull(entityA, entityB, manifold);
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}

		if (collision)
		{
			manifolds.push_back(manifold);
		}
	}

	// Resolve collisions

	for (int i = 0; i < manifolds.size(); ++i)
	{
		const Manifold& manifold = manifolds[i];

		const Entity& entityA = manifold.entityA;
		const Entity& entityB = manifold.entityB;

		ResolveManifold(manifold, entityA, entityB, em);
	}
}

// Find vertex with most penetration
// Plane MUST be in hull space
float GetSeperationDepth(const gMath::Plane& testPlane, const ConvexHull& hull)
{
	float minSeperation = FLT_MAX;

	for (int v = 0; v < hull.vertCount; ++v)
	{
		glm::vec3 point = hull.verts[v].position;

		float dot = glm::dot(point, testPlane.normal);

		if (dot <= minSeperation)
		{
			minSeperation = dot;
		}
	}

	minSeperation -= testPlane.dist;

	return minSeperation;
}

FaceQuery SatFaceTest(const HullCollider& hullA, const glm::vec3& positionA, const glm::fquat& rotationA,
	const HullCollider& hullB, const glm::vec3& positionB, const glm::fquat& rotationB)
{
	gMath::Plane testPlane;
	FaceQuery query = FaceQuery();
	query.seperation = -FLT_MAX;

	for (int f = 0; f < hullB.pHull->faceCount; ++f)
	{
		testPlane = hullB.pHull->faces[f].plane;

		// TODO: Should be a way to skip converting to world space first

		// World space
		glm::vec3 oldNormal = testPlane.normal;
		glm::vec3 normal1 = rotationB * oldNormal;
		testPlane.dist = -glm::dot(-(positionB + normal1 * testPlane.dist), normal1);

		// A space
		testPlane.dist = -glm::dot(positionA - (normal1 * testPlane.dist), normal1);
		testPlane.normal = glm::inverse(rotationA) * normal1;

		float seperation = GetSeperationDepth(testPlane, *hullB.pHull);

		if (seperation > query.seperation)
		{
			query.seperation = seperation;
			query.pFace = &hullA.pHull->faces[f];
		}

		// TODO: Early exit could cause problems?
		if (seperation > 0)
		{
			return query;
		}
	}

	return query;
}

bool NormalsBuildMinkowskiFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& b_x_a,
	const glm::vec3& c, const glm::vec3& d, const glm::vec3& d_x_c)
{
	float cba = glm::dot(c, b_x_a);
	float dba = glm::dot(d, b_x_a);
	float adc = glm::dot(a, d_x_c);
	float bdc = glm::dot(b, d_x_c);

	return cba * dba < 0 && adc * bdc < 0 && cba * bdc > 0;
}

bool EdgesBuildMinkowskiFace(const qhEdge& edgeA, const qhEdge& edgeB,
	const glm::vec3& edgeADir, const glm::vec3& edgeBDir,
	const glm::fquat& rotationA, const glm::fquat& rotationB)
{
	glm::vec3 a = rotationA * edgeA.pHalfA->pFace->plane.normal;
	glm::vec3 b = rotationA * edgeA.pHalfB->pFace->plane.normal;
	glm::vec3 c = rotationB * edgeB.pHalfA->pFace->plane.normal;
	glm::vec3 d = rotationB * edgeB.pHalfB->pFace->plane.normal;

	return NormalsBuildMinkowskiFace(a, b, edgeADir, -c, -d, edgeBDir);
}

EdgeQuery SatEdgeTest(const HullCollider& hullA, const glm::vec3& positionA, const glm::fquat& rotationA,
	const HullCollider& hullB, const glm::vec3& positionB, const glm::fquat& rotationB)
{
	EdgeQuery query;
	query.seperation = -FLT_MAX;

	glm::vec3 bOffset = positionB - positionA;

	// Check edge combinations
	for (int edgeAIndex = 0; edgeAIndex < hullA.pHull->edgeCount; ++edgeAIndex)
	{
		glm::vec3 edgeATail = hullA.pHull->edges[edgeAIndex].pHalfA->pTail->position;
		glm::vec3 edgeADir = hullA.pHull->edges[edgeAIndex].pHalfB->pTail->position - edgeATail;

		edgeATail = rotationA * edgeATail + positionA;
		edgeADir = rotationA * edgeADir;

		for (int edgeBIndex = 0; edgeBIndex < hullB.pHull->edgeCount; ++edgeBIndex)
		{
			glm::vec3 edgeBTail = hullB.pHull->edges[edgeBIndex].pHalfA->pTail->position;
			glm::vec3 edgeBDir = hullB.pHull->edges[edgeBIndex].pHalfB->pTail->position - edgeBTail;

			edgeBTail = rotationB * edgeBTail + bOffset;
			edgeBDir = rotationB * edgeBDir;

			if (!EdgesBuildMinkowskiFace(hullA.pHull->edges[edgeAIndex], hullB.pHull->edges[edgeBIndex],
				edgeADir, edgeBDir, rotationA, rotationB))
			{
				continue;
			}

			gMath::Plane testPlane;
			testPlane.normal = glm::normalize(glm::cross(edgeADir, edgeBDir));

			// Check if the normal is facing the right direction
			if (glm::dot(edgeATail - positionA, testPlane.normal) < 0)
			{
				testPlane.normal = -testPlane.normal;
			}

			// Move plane to edge
			testPlane.dist = glm::dot(edgeATail, testPlane.normal);

			// Move plane to Hull B space
			gMath::Plane bPlane;
			bPlane.dist = glm::dot(testPlane.normal * testPlane.dist - positionB, testPlane.normal);
			bPlane.normal = glm::inverse(rotationB) * testPlane.normal;

			float seperation = GetSeperationDepth(bPlane, *hullB.pHull);

			if (seperation >= query.seperation)
			{
				query.seperation = seperation;
				query.pEdgeA = &hullA.pHull->edges[edgeAIndex];
				query.pEdgeB = &hullB.pHull->edges[edgeBIndex];

				query.normal = testPlane.normal;
			}

			if (seperation > 0)
			{
				return query;
			}
		}
	}

	return query;
}

float ProjectPointToLine(const glm::vec3& point, const gMath::Line& line)
{
	glm::vec3 lineBProjDir = line.pointB - line.pointA;
	float interp = glm::dot(point - line.pointA, lineBProjDir) / glm::dot(lineBProjDir, lineBProjDir);

	return interp;
}

glm::vec3 GetClosestPointOnLine(const gMath::Line& lineA, const gMath::Line& lineB)
{
	// Project line B onto the plane of line A
	glm::vec3 normal = lineA.pointB - lineA.pointA;
	normal = glm::normalize(normal);

	gMath::Line lineBProj = lineB;
	{
		float pointADot = glm::dot(lineB.pointA, normal);
		float pointBDot = glm::dot(lineB.pointB, normal);

		lineBProj.pointA -= normal * pointADot;
		lineBProj.pointB -= normal * pointBDot;
	}

	// Project the first point of line A onto the projected line B
	float t = ProjectPointToLine(lineA.pointA, lineBProj);
	glm::vec3 point = gMath::Lerp(lineB.pointA, lineB.pointB, t);

	return point;
}

void CreateEdgeContacts(const EdgeQuery& query, const glm::vec3& positionA, const glm::fquat& rotationA,
	const glm::vec3& positionB, const glm::fquat& rotationB, Manifold& manifold)
{
	gMath::Line lineA = {
		query.pEdgeA->pHalfA->pTail->position,
		query.pEdgeA->pHalfB->pTail->position,
	};

	lineA.pointA = rotationA * lineA.pointA + positionA;
	lineA.pointB = rotationA * lineA.pointB + positionA;

	gMath::Line lineB = {
		query.pEdgeB->pHalfA->pTail->position,
		query.pEdgeB->pHalfB->pTail->position,
	};

	lineB.pointA = rotationB * lineB.pointA + positionB;
	lineB.pointB = rotationB * lineB.pointB + positionB;

	glm::vec3 pointA = GetClosestPointOnLine(lineA, lineB);
	glm::vec3 pointB = GetClosestPointOnLine(lineB, lineA);

	glm::vec3 average = (pointA - pointB) / 2.0f + pointB;

	ContactPoint contact;
	contact.position = average;

	manifold.contacts[0] = contact;
	manifold.numContacts = 1;
}

bool PhysicsSystem::HullVsHull(Entity& entityA, Entity& entityB, Manifold& manifold)
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

	FaceQuery faceQueryA = SatFaceTest(hullA, positionA.value, rotationA.value, hullB, positionB.value, rotationB.value);

	// Check faces of A
	if (faceQueryA.seperation > 0)
	{
		return false;
	}

	FaceQuery faceQueryB = SatFaceTest(hullB, positionB.value, rotationB.value, hullA, positionA.value, rotationA.value);

	// Check faces of B
	if (faceQueryB.seperation > 0)
	{
		return false;
	}

	EdgeQuery edgeQuery = SatEdgeTest(hullA, positionA.value, rotationA.value, hullB, positionB.value, rotationB.value);

	// Check edge combinations
	if (edgeQuery.seperation > 0)
	{
		return false;
	}

	manifold.entityA = entityA;
	manifold.entityB = entityB;

	bool isFaceContactA = faceQueryA.seperation > edgeQuery.seperation;
	bool isFaceContactB = faceQueryB.seperation > edgeQuery.seperation;

	float seperation;

	if (isFaceContactA && isFaceContactB)
	{
		bool aIsBiggerThanB = faceQueryA.seperation > faceQueryB.seperation;

#ifdef PHYS_DEBUG
		if (aIsBiggerThanB)
		{
			gMath::Plane drawPlane = faceQueryA.pFace->plane;
			drawPlane.normal = rotationA.value * drawPlane.normal;
			debugDraw.DrawPlane(positionA.value, drawPlane, 1.5f, 1.5f, {1, 1, 0});
		}
		else
		{
			gMath::Plane drawPlane = faceQueryB.pFace->plane;
			drawPlane.normal = rotationB.value * drawPlane.normal;
			debugDraw.DrawPlane(positionB.value, drawPlane, 1.5f, 1.5f, { 1, 1, 0 });
		}
#endif // PHYS_DEBUG

		if (aIsBiggerThanB)
		{
			seperation = faceQueryA.seperation;
		}
		else
		{
			seperation = faceQueryB.seperation;
		}

	}
	else
	{
		manifold.normal = edgeQuery.normal;
		CreateEdgeContacts(edgeQuery, positionA.value, rotationA.value, positionB.value, rotationB.value, manifold);
		seperation = edgeQuery.seperation;

#ifdef PHYS_DEBUG
		gMath::Plane drawPlane;
		drawPlane.normal = edgeQuery.normal;
		glm::vec3 edgePos = rotationA.value * edgeQuery.pEdgeA->pHalfA->pTail->position;
		drawPlane.dist = glm::dot(edgePos, edgeQuery.normal);

		debugDraw.DrawPlane(positionA.value, drawPlane, 1.5f, 1.5f, {0, 1, 1});
#endif // PHYS_DEBUG

	}

	manifold.seperation = seperation;

#ifdef PHYS_DEBUG
	std::cout << "Collide: " << seperation << "\n";
#endif // PHYS_DEBUG

	return true;
}