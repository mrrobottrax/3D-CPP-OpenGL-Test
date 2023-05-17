#include <pch.h>
#include <systems/physicssystem.h>
#include <systems/systemmanager.h>
#include <debugtools/debugdraw.h>

#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/masscomponent.h>
#include <components/idcomponent.h>
#include <components/hullcollider.h>
#include <components/scalecomponent.h>

#include <common/math.h>

using namespace gmath;

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

void ResolveManifold(const Manifold& manifold, const Entity& entityA, const Entity& entityB)
{
	const EntityManager& em = entityManager;

	const PositionComponent& positionA = em.GetComponent<PositionComponent>(entityA);
	const PositionComponent& positionB = em.GetComponent<PositionComponent>(entityB);

	const float& inv_massA = em.GetComponent<MassComponent>(entityA).inv_mass;
	const float& inv_inertiaA = em.GetComponent<MassComponent>(entityA).inv_inertia;
	const float& inv_massB = em.GetComponent<MassComponent>(entityB).inv_mass;
	const float& inv_inertiaB = em.GetComponent<MassComponent>(entityB).inv_inertia;

	// Unstoppable force vs immovable object
	if (inv_massA == 0 && inv_massB == 0)
		return;

	// Don't allow zero mass
	if (inv_massA == INFINITY || inv_inertiaA == INFINITY || inv_massB == INFINITY || inv_inertiaB == INFINITY)
		return;

	VelocityComponent& velocityA = em.GetComponent<VelocityComponent>(entityA);
	VelocityComponent& velocityB = em.GetComponent<VelocityComponent>(entityB);

	const glm::vec3& normal = manifold.normal;

	float jacobians[4][12]{};
	float inverseEffectiveMasses[4]{};
	float totalImpulses[4]{};

	glm::vec3 crossA[4]{};
	glm::vec3 crossB[4]{};

	for (int i = 0; i < manifold.numContacts; ++i)
	{
		totalImpulses[i] = 0;
		const ContactPoint& contact = manifold.contacts[i];

		// Calculate jacobian matrix
		jacobians[i][0] = -normal.x;
		jacobians[i][1] = -normal.y;
		jacobians[i][2] = -normal.z;

		crossA[i] = glm::cross(contact.position - positionA.value, normal);
		jacobians[i][3] = crossA[i].x;
		jacobians[i][4] = crossA[i].y;
		jacobians[i][5] = crossA[i].z;

		jacobians[i][6] = normal.x;
		jacobians[i][7] = normal.y;
		jacobians[i][8] = normal.z;

		crossB[i] = glm::cross(contact.position - positionB.value, normal);
		jacobians[i][9] = crossB[i].x;
		jacobians[i][10] = crossB[i].y;
		jacobians[i][11] = crossB[i].z;

		// Calculate relative mass
		{
			float n = glm::dot(normal, normal);
			float cA = glm::dot(crossA[i], crossA[i]);
			float cB = glm::dot(crossB[i], crossB[i]);
			inverseEffectiveMasses[i] = 1.0f / ((n * inv_massA) + (n * inv_massB) +
				(cA * inv_inertiaA) + (cB * inv_inertiaB));
		}
	}

	const float b = manifold.seperation * 0.8f;
	for (int j = 0; j < numIterations; ++j)
	{
		for (int i = 0; i < manifold.numContacts; ++i)
		{
			const float oldImpulse = totalImpulses[i];

			const glm::vec3& vA = velocityA.linear;
			const glm::vec3& vB = velocityB.linear;
			const glm::vec3& wA = velocityA.angular;
			const glm::vec3& wB = velocityB.angular;

			const float velocityAtPoint = glm::dot(-normal, vA) + glm::dot(-crossA[i], wA)
				+ glm::dot(normal, vB) + glm::dot(crossB[i], wB);
			totalImpulses[i] = totalImpulses[i] - (inverseEffectiveMasses[i] * (velocityAtPoint + b));

			// Enforce Signorini conditions
			if (totalImpulses[i] < 0)
				totalImpulses[i] = 0;

			const float deltaImpulse = totalImpulses[i] - oldImpulse;

			// Add velocity
			velocityA.linear -= normal * inv_massA * deltaImpulse * timescale.value;
			velocityA.angular -= crossA[i] * inv_inertiaA * deltaImpulse * timescale.value;

			velocityB.linear += normal * inv_massB * deltaImpulse * timescale.value;
			velocityB.angular += crossB[i] * inv_inertiaB * deltaImpulse * timescale.value;

			// Friction

#ifdef FRICTION_DEBUG
			{
				const glm::vec3& pos = manifold.contacts[i].position;
				debugDraw.DrawLine(pos, pos + manifold.cross1 * 0.2f, { 1, 0, 0 });
				debugDraw.DrawLine(pos, pos + manifold.cross2 * 0.2f, { 0, 1, 0 });
				debugDraw.DrawLine(pos, pos + manifold.normal * 0.2f, { 0, 0, 1 });
			}
#endif // FRICTION_DEBUG

		}
	}
}

void PhysicsSystem::Update()
{
	EntityManager& em = entityManager;

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
				const Entity entity((*chunkArchetypeIt)->archetype, *pChunk, i);

				// Gravity
				RigidBodyComponent& rb = em.GetComponent<RigidBodyComponent>(entity);
				VelocityComponent& vel = em.GetComponent<VelocityComponent>(entity);
				vel.linear.y += rb.gravityScale * gravity * timeManager.GetDeltaTime();
			}
		}
	}

	// Broad phase ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

				const Entity entity((*chunkArchetypeIt)->archetype, *pChunk, i);
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
							const Entity entity2((*chunkArchetypeIt2)->archetype, *pChunk2, i2);
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

		PositionComponent* positionA = entityManager.GetComponentP<PositionComponent>(entityA);

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

#ifdef CONTACT_DEBUG
		for (int i = 0; i < manifold.numContacts; ++i)
		{
			debugDraw.DrawLine(manifold.contacts[i].position, manifold.contacts[i].position + manifold.normal * 0.2f, {1, 0, 1});
		}
#endif // CONTACT_DEBUG

		ResolveManifold(manifold, entityA, entityB);
	}
}

// Find vertex with most penetration
// Plane MUST be in hull space
float GetSeperationDepth(gmath::Plane testPlane, const ConvexHull& hull, const glm::vec3& scale)
{

#ifdef SAT_DEBUG
	debugDraw.DrawPlane(glm::vec3(0), testPlane, 1.5f, 1.5f, { 1, 0, 0 });
#endif // SAT_DEBUG

	testPlane.normal *= scale;

	float minSeperation = FLT_MAX;

	for (int v = 0; v < hull.vertCount; ++v)
	{
		glm::vec3 point = hull.verts[v].position;

		float dot = glm::dot(point, testPlane.normal);

		if (dot <= minSeperation)
		{
			minSeperation = dot;
		}

#ifdef SAT_DEBUG
		debugDraw.DrawLine(point, point + glm::vec3(0, 0.1f, 0), { 0, 0, 1 });
#endif // SAT_DEBUG

	}

	minSeperation -= testPlane.dist;

	return minSeperation;
}

void PlaneToWorld(Plane& plane, const glm::vec3& spacePosition, const glm::fquat& spaceRotation)
{
	plane.normal = spaceRotation * plane.normal;
	plane.dist += glm::dot(spacePosition, plane.normal);
}

void PlaneToSpace(Plane& plane, const glm::vec3& spacePosition, const glm::fquat& spaceRotation)
{
	plane.dist -= glm::dot(spacePosition, plane.normal);
	plane.normal = glm::inverse(spaceRotation) * plane.normal;
}

FaceQuery SatFaceTest(const HullCollider& hullA, const glm::vec3& positionA, const glm::fquat& rotationA, const glm::vec3& scaleA,
	const HullCollider& hullB, const glm::vec3& positionB, const glm::fquat& rotationB, const glm::vec3& scaleB)
{
	gmath::Plane testPlane;
	FaceQuery query = FaceQuery();
	query.seperation = -FLT_MAX;

	for (int f = 0; f < hullA.pHull->faceCount; ++f)
	{
		testPlane = hullA.pHull->faces[f].plane;

		ScalePlane(testPlane, scaleA);
		PlaneToWorld(testPlane, positionA, rotationA);
		PlaneToSpace(testPlane, positionB, rotationB);

#ifdef SAT_DEBUG
		Plane drawPlane = testPlane;
		PlaneToWorld(drawPlane, positionB, rotationB);
		PlaneToSpace(drawPlane, positionA, rotationA);
		PlaneToWorld(drawPlane, glm::vec3(0), rotationA);
		debugDraw.DrawPlane(positionA, drawPlane, 1.5f, 1.5f, glm::vec3(0.5f));
#endif // SAT_DEBUG

		float seperation = GetSeperationDepth(testPlane, *hullB.pHull, scaleB);

		if (seperation > query.seperation)
		{
			query.seperation = seperation;
			query.pFace = &hullA.pHull->faces[f];
		}

		if (seperation > 0)
		{
			return query;
		}
	}

#ifdef SAT_DEBUG
	Plane drawPlane = query.pFace->plane;
	ScalePlane(drawPlane, scaleA);
	PlaneToWorld(drawPlane, glm::vec3(0), rotationA);
	debugDraw.DrawPlane(positionA, drawPlane, 1.5f, 1.5f, glm::vec3(1, 0.5f, 0));
#endif // SAT_DEBUG

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

bool EdgesBuildMinkowskiFace(const qhEdge& edgeA, const glm::vec3& edgeADir, const glm::fquat& rotationA,
	const qhEdge& edgeB, const glm::vec3& edgeBDir, const glm::fquat& rotationB)
{
	glm::vec3 a = rotationA * edgeA.pHalfA->pFace->plane.normal;
	glm::vec3 b = rotationA * edgeA.pHalfB->pFace->plane.normal;
	glm::vec3 c = rotationB * edgeB.pHalfA->pFace->plane.normal;
	glm::vec3 d = rotationB * edgeB.pHalfB->pFace->plane.normal;

	return NormalsBuildMinkowskiFace(a, b, edgeADir, -c, -d, edgeBDir);
}

EdgeQuery SatEdgeTest(const HullCollider& hullA, const glm::vec3& positionA, const glm::fquat& rotationA, const glm::vec3& scaleA,
	const HullCollider& hullB, const glm::vec3& positionB, const glm::fquat& rotationB, const glm::vec3& scaleB)
{
	EdgeQuery query;
	query.seperation = -FLT_MAX;

	// Check edge combinations
	for (int edgeAIndex = 0; edgeAIndex < hullA.pHull->edgeCount; ++edgeAIndex)
	{
		glm::vec3 edgeATail = hullA.pHull->edges[edgeAIndex].pHalfA->pTail->position;
		glm::vec3 edgeADir = hullA.pHull->edges[edgeAIndex].pHalfB->pTail->position - edgeATail;

		edgeATail = rotationA * (edgeATail * scaleA) + positionA;
		edgeADir = rotationA * (edgeADir * scaleA);

		for (int edgeBIndex = 0; edgeBIndex < hullB.pHull->edgeCount; ++edgeBIndex)
		{
			glm::vec3 edgeBTail = hullB.pHull->edges[edgeBIndex].pHalfA->pTail->position;
			glm::vec3 edgeBDir = hullB.pHull->edges[edgeBIndex].pHalfB->pTail->position - edgeBTail;

			edgeBTail = rotationB * (edgeBTail * scaleB) + positionB;
			edgeBDir = rotationB * (edgeBDir * scaleB);

			if (!EdgesBuildMinkowskiFace(hullA.pHull->edges[edgeAIndex], edgeADir, rotationA,
				hullB.pHull->edges[edgeBIndex], edgeBDir, rotationB))
			{
				continue;
			}

			gmath::Plane testPlane;
			testPlane.normal = glm::normalize(glm::cross(edgeADir, edgeBDir));

			// Check if the normal is facing the right direction
			if (glm::dot(edgeATail - positionA, testPlane.normal) < 0)
			{
				testPlane.normal = -testPlane.normal;
			}

			// Move plane to edge
			testPlane.dist = glm::dot(edgeATail, testPlane.normal);

#ifdef SAT_DEBUG
			Plane drawPlane = testPlane;
			PlaneToSpace(drawPlane, positionA, rotationA);
			PlaneToWorld(drawPlane, glm::vec3(0), rotationA);
			debugDraw.DrawPlane(positionA, drawPlane, 0.2f, 0.2f, { 0, 0.5f, 0 });
#endif // SAT_DEBUG

			PlaneToSpace(testPlane, positionB, rotationB);
			float seperation = GetSeperationDepth(testPlane, *hullB.pHull, scaleB);

			if (seperation >= query.seperation)
			{
				query.seperation = seperation;
				query.pEdgeA = &hullA.pHull->edges[edgeAIndex];
				query.pEdgeB = &hullB.pHull->edges[edgeBIndex];

				query.normal = rotationB * testPlane.normal;
			}

			if (seperation > 0)
			{
				return query;
			}
		}
	}

	return query;
}

float ProjectPointToLine(const glm::vec3& point, const gmath::Line& line)
{
	glm::vec3 lineBProjDir = line.pointB - line.pointA;
	float interp = glm::dot(point - line.pointA, lineBProjDir) / glm::dot(lineBProjDir, lineBProjDir);

	return interp;
}

glm::vec3 GetClosestPointOnLine(const gmath::Line& lineA, const gmath::Line& lineB)
{
	// Project line B onto the plane of line A
	glm::vec3 normal = lineA.pointB - lineA.pointA;
	normal = glm::normalize(normal);

	gmath::Line lineBProj = lineB;
	{
		float pointADot = glm::dot(lineB.pointA, normal);
		float pointBDot = glm::dot(lineB.pointB, normal);

		lineBProj.pointA -= normal * pointADot;
		lineBProj.pointB -= normal * pointBDot;
	}

	// Project the first point of line A onto the projected line B
	float t = ProjectPointToLine(lineA.pointA, lineBProj);
	glm::vec3 point = gmath::Lerp(lineB.pointA, lineB.pointB, t);

	return point;
}

void CreateEdgeContacts(const EdgeQuery& query, const glm::vec3& positionA, const glm::fquat& rotationA, const glm::vec3& scaleA,
	const glm::vec3& positionB, const glm::fquat& rotationB, const glm::vec3& scaleB, Manifold& manifold)
{
	gmath::Line lineA = {
		query.pEdgeA->pHalfA->pTail->position * scaleA,
		query.pEdgeA->pHalfB->pTail->position * scaleA,
	};

	lineA.pointA = rotationA * lineA.pointA + positionA;
	lineA.pointB = rotationA * lineA.pointB + positionA;

	gmath::Line lineB = {
		query.pEdgeB->pHalfA->pTail->position * scaleB,
		query.pEdgeB->pHalfB->pTail->position * scaleB,
	};

	lineB.pointA = rotationB * lineB.pointA + positionB;
	lineB.pointB = rotationB * lineB.pointB + positionB;

	// Calculate line directions
	glm::vec3 dirA = lineA.pointB - lineA.pointA;
	manifold.cross1 = dirA;
	manifold.cross2 = glm::cross(dirA, manifold.normal);
	manifold.cross1 = glm::normalize(manifold.cross1);
	manifold.cross2 = glm::normalize(manifold.cross2);

#ifdef CONTACT_DEBUG
	debugDraw.DrawLine(lineA.pointA, lineA.pointB, { 1, 1, 0 });
	debugDraw.DrawLine(lineB.pointA, lineB.pointB, { 1, 1, 0 });
#endif // CONTACT_DEBUG

	glm::vec3 pointA = GetClosestPointOnLine(lineA, lineB);
	glm::vec3 pointB = GetClosestPointOnLine(lineB, lineA);

	glm::vec3 average = (pointA - pointB) / 2.0f + pointB;

	ContactPoint contact;
	contact.position = average;

	manifold.contacts[0] = contact;
	manifold.numContacts = 1;
}

// Find most anti-parallel face
// Normal must be in hull space
qhFace& FindIncidentFace(const glm::vec3& normal, const ConvexHull& hull)
{
	// TODO: Walking adjacent faces would be faster
	// Like finding support point

	qhFace* pFace = nullptr;
	float bestDot = FLT_MAX;

	for (int i = 0; i < hull.faceCount; ++i)
	{
		const qhFace& face = hull.faces[i];

		const float dot = glm::dot(normal, face.plane.normal);

		if (dot <= bestDot)
		{
			bestDot = dot;
			pFace = const_cast<qhFace*>(&face);
		}
	}

	return *pFace;
}

void CreateFaceContacts(const FaceQuery& queryA, const glm::vec3& positionA, const glm::fquat& rotationA, const glm::vec3& scaleA,
	const ConvexHull& hullB, const glm::vec3& positionB, const glm::fquat& rotationB, const glm::vec3& scaleB, Manifold& manifold)
{
	const qhFace& referenceFace = *queryA.pFace;

	Plane relativeReferencePlane = referenceFace.plane;
	ScalePlane(relativeReferencePlane, scaleA);

	PlaneToWorld(relativeReferencePlane, positionA, rotationA);
	PlaneToSpace(relativeReferencePlane, positionB, rotationB);

	const qhFace& incidentFace = FindIncidentFace(relativeReferencePlane.normal, hullB);

	// Find friction vectors
	manifold.cross1 = rotationA * (scaleA * queryA.pFace->pEdge->pTail->position - scaleB * queryA.pFace->pEdge->pTwin->pTail->position);
	manifold.cross2 = glm::cross(manifold.cross1, manifold.normal);
	manifold.cross1 = glm::normalize(manifold.cross1);
	manifold.cross2 = glm::normalize(manifold.cross2);

	// Clip with Sutherland-Hodgman
	std::vector<glm::vec3> in;
	std::vector<glm::vec3> out;

	// Get starting verts
	{
		const qhHalfEdge* startEdge = incidentFace.pEdge;
		qhHalfEdge* pEdge = const_cast<qhHalfEdge*>(startEdge);
		do
		{
			in.push_back(pEdge->pTail->position * scaleB);

			pEdge = pEdge->pNext;
		} while (pEdge != startEdge);
	}

	// Clip against adjacent planes
	bool swapBuffers = false;
	{
		const qhHalfEdge* startEdge = referenceFace.pEdge;
		qhHalfEdge* pEdge = const_cast<qhHalfEdge*>(startEdge);
		do
		{
			std::vector<glm::vec3>& inBuffer = swapBuffers ? out : in;
			std::vector<glm::vec3>& outBuffer = !swapBuffers ? out : in;

			const qhFace& clipFace = *pEdge->pTwin->pFace;
			Plane clipPlane = clipFace.plane;
			ScalePlane(clipPlane, scaleA);

			PlaneToWorld(clipPlane, positionA, rotationA);
			PlaneToSpace(clipPlane, positionB, rotationB);

#ifdef CONTACT_DEBUG
			Plane drawPlane = clipPlane;
			PlaneToWorld(drawPlane, glm::vec3(0), rotationB);
			debugDraw.DrawPlane(positionB, drawPlane, 1.5f, 1.5f, { 1, 1, 1 });
#endif // CONTACT_DEBUG

			// Keep vertices behind plane
			for (int i = 0; i < inBuffer.size(); ++i)
			{
				const glm::vec3& point = inBuffer[i];
				const float dist = glm::dot(point, clipPlane.normal) - clipPlane.dist;

#ifdef CONTACT_DEBUG
				glm::vec3 drawPoint = rotationB * point + positionB;
				debugDraw.DrawLine(drawPoint, drawPoint + manifold.normal * 0.2f, { 0, 0.1f, 0 });
#endif // CONTACT_DEBUG

				const bool pointBehindPlane = dist <= 0;

				const int j = i - 1;
				const glm::vec3& prev = inBuffer[j < 0 ? inBuffer.size() - 1 : j];

				const bool lineCrossesPlane = (glm::dot(prev, clipPlane.normal) - clipPlane.dist <= 0) != pointBehindPlane;
				if (lineCrossesPlane)
				{
					// Find intersection of line and plane
					const Line line = {
						prev,
						point
					};

					const glm::vec3 point = LineAndPlane(line, clipPlane);
					outBuffer.push_back(point);

#ifdef CONTACT_DEBUG
					debugDraw.DrawPlane(glm::vec3(0), clipPlane, 1.5f, 1.5f);
					debugDraw.DrawLine(line.pointA, line.pointB, { 1, 1, 1 });

					glm::vec3 drawPoint = point;
					drawPoint = rotationB * drawPoint + positionB;
					debugDraw.DrawLine(drawPoint, drawPoint + glm::vec3(0, 0.2f, 0), { 1, 1, 1 });
#endif // CONTACT_DEBUG

				}

				// Added after to maintain sequence
				if (pointBehindPlane)
				{
					outBuffer.push_back(point);
				}
			}

			inBuffer.clear();
			swapBuffers = !swapBuffers;

			pEdge = pEdge->pNext;
		} while (pEdge != startEdge);
	}

	// Remove contacts above the reference face
	{
		std::vector<glm::vec3>& inBuffer = swapBuffers ? out : in;
		std::vector<glm::vec3>& outBuffer = !swapBuffers ? out : in;
		for (int i = 0; i < inBuffer.size(); ++i)
		{
			const glm::vec3& point = inBuffer[i];
			const float dist = glm::dot(relativeReferencePlane.normal, point) - relativeReferencePlane.dist;

			if (dist <= 0)
			{
				outBuffer.push_back(point);
			}
		}

		swapBuffers = !swapBuffers;
		inBuffer.clear();
	}

	// Project onto reference plane
	{
		std::vector<glm::vec3>& inBuffer = swapBuffers ? out : in;

		for (int i = 0; i < inBuffer.size(); ++i)
		{
			glm::vec3& point = inBuffer[i];
			const float dist = glm::dot(relativeReferencePlane.normal, point) - relativeReferencePlane.dist;

			point -= relativeReferencePlane.normal * dist;
		}
	}

	// Reduce contact points
	{
		std::vector<glm::vec3>& inBuffer = swapBuffers ? out : in;

		if (inBuffer.size() > 4)
		{
			std::vector<glm::vec3>& outBuffer = !swapBuffers ? out : in;

			// TODO: First point should be deepest for continuous collision
			glm::vec3* firstPoint = &inBuffer[0];
			{
				const glm::vec3 direction = glm::vec3(1, 1, 1);

				float bestDot = -FLT_MAX;
				for (int i = 1; i < inBuffer.size(); ++i)
				{
					glm::vec3& point = inBuffer[i];
					const float dot = glm::dot(point, direction);

					if (dot >= bestDot)
					{
						firstPoint = const_cast<glm::vec3*>(&point);
						bestDot = dot;
					}
				}
			}

			// Get furthest point
			glm::vec3* secondPoint = nullptr;
			{
				float bestDist = -FLT_MAX;
				for (int i = 0; i < inBuffer.size(); ++i)
				{
					const glm::vec3* pPoint = &inBuffer[i];

					if (pPoint == firstPoint)
						continue;

					const glm::vec3 tmp = *pPoint - *firstPoint;
					const float sqrDist = SqrDist(*pPoint, *firstPoint);

					if (sqrDist >= bestDist)
					{
						secondPoint = const_cast<glm::vec3*>(pPoint);
						bestDist = sqrDist;
					}
				}
			}

			auto GetArea = [&manifold](const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& pointC)
			{
				const glm::vec3 ca = pointA - pointC;
				const glm::vec3 cb = pointB - pointC;

				return 0.5f * glm::dot(glm::cross(ca, cb), manifold.normal);
			};

			// Get third point
			glm::vec3* thirdPoint = nullptr;
			bool thirdSign = 0;
			{
				float bestArea = 0;
				for (int i = 0; i < inBuffer.size(); ++i)
				{
					const glm::vec3* pPoint = &inBuffer[i];

					if (pPoint == firstPoint)
						continue;

					if (pPoint == secondPoint)
						continue;

					assert(secondPoint != nullptr);

					const float area = GetArea(*firstPoint, *secondPoint, *pPoint);

					if (abs(area) >= abs(bestArea))
					{
						thirdPoint = const_cast<glm::vec3*>(pPoint);
						bestArea = area;
						thirdSign = signbit(area);
					}
				}
			}

			// Get fourth point
			glm::vec3* fourthPoint = nullptr;
			bool fourthSign = 0;
			{
				float bestArea = 0;
				for (int i = 0; i < inBuffer.size(); ++i)
				{
					const glm::vec3* pPoint = &inBuffer[i];

					if (pPoint == firstPoint)
						continue;

					if (pPoint == secondPoint)
						continue;

					assert(secondPoint != nullptr);

					const float area = GetArea(*firstPoint, *secondPoint, *pPoint);

					bool sign = signbit(area);
					if (abs(area) >= abs(bestArea) && sign != thirdSign)
					{
						fourthPoint = const_cast<glm::vec3*>(pPoint);
						bestArea = area;
						fourthSign = sign;
					}
				}
			}

			assert(firstPoint != nullptr);
			assert(secondPoint != nullptr);
			assert(thirdPoint != nullptr);
			assert(fourthPoint != nullptr);

			outBuffer.push_back(*firstPoint);
			outBuffer.push_back(*secondPoint);
			outBuffer.push_back(*thirdPoint);
			outBuffer.push_back(*fourthPoint);

			swapBuffers = !swapBuffers;
			inBuffer.clear();
		}
	}

	// Move contacts to world space
	{
		const std::vector<glm::vec3>& inBuffer = swapBuffers ? out : in;
		{
			manifold.numContacts = (int)inBuffer.size();
			for (int i = 0; i < inBuffer.size(); ++i)
			{
				manifold.contacts[i] = {
					rotationB * inBuffer[i] + positionB
				};
			}
		}
	}
}

bool PhysicsSystem::HullVsHull(Entity& entityA, Entity& entityB, Manifold& manifold)
{
	// Seperating Axis Theorum

	EntityManager& em = entityManager;

	PositionComponent& positionA = em.GetComponent<PositionComponent>(entityA);
	PositionComponent& positionB = em.GetComponent<PositionComponent>(entityB);
	RotationComponent& rotationA = em.GetComponent<RotationComponent>(entityA);
	RotationComponent& rotationB = em.GetComponent<RotationComponent>(entityB);

	const HullCollider& hullA = em.GetComponent<HullCollider>(entityA);
	const HullCollider& hullB = em.GetComponent<HullCollider>(entityB);

	glm::vec3 scaleA = glm::vec3(1);
	glm::vec3 scaleB = glm::vec3(1);
	{
		const ScaleComponent* pScaleA = em.GetComponentP<ScaleComponent>(entityA);
		const ScaleComponent* pScaleB = em.GetComponentP<ScaleComponent>(entityB);

		if (pScaleA)
			scaleA = pScaleA->value;
		if (pScaleB)
			scaleB = pScaleB->value;
	}

#ifdef PHYS_DEBUG

	auto DrawHull = [](const ConvexHull* pHull, const glm::vec3& position,
		const glm::fquat& rotation, const glm::vec3& scale, const glm::vec3& color)
	{
		for (int i = 0; i < pHull->edgeCount; ++i)
		{
			const qhEdge& edge = pHull->edges[i];

			glm::vec3 start = edge.pHalfA->pTail->position * scale;
			glm::vec3 end = edge.pHalfB->pTail->position * scale;

			start = rotation * start + position;
			end = rotation * end + position;

			debugDraw.DrawLine(start, end, color);
		}
	};

	DrawHull(hullA.pHull, positionA.value, rotationA.value, scaleA, {1, 0, 0});
	DrawHull(hullB.pHull, positionB.value, rotationB.value, scaleB, {0, 0, 1});

#endif // PHYS_DEBUG

	FaceQuery faceQueryA = SatFaceTest(hullA, positionA.value, rotationA.value, scaleA, hullB, positionB.value, rotationB.value, scaleB);

	// Check faces of A
	if (faceQueryA.seperation > 0)
	{
		return false;
	}

	FaceQuery faceQueryB = SatFaceTest(hullB, positionB.value, rotationB.value, scaleB, hullA, positionA.value, rotationA.value, scaleA);

	// Check faces of B
	if (faceQueryB.seperation > 0)
	{
		return false;
	}

	EdgeQuery edgeQuery = SatEdgeTest(hullA, positionA.value, rotationA.value, scaleA, hullB, positionB.value, rotationB.value, scaleB);

	// Check edge combinations
	if (edgeQuery.seperation > 0)
	{
		return false;
	}

	manifold.entityA = entityA;
	manifold.entityB = entityB;

	bool isFaceContactA = faceQueryA.seperation >= edgeQuery.seperation;
	bool isFaceContactB = faceQueryB.seperation >= edgeQuery.seperation;

	if (isFaceContactA || isFaceContactB)
	{
		bool aIsBiggerThanB = faceQueryA.seperation > faceQueryB.seperation;

#ifdef SAT_DEBUG
		if (aIsBiggerThanB)
		{
			gmath::Plane drawPlane = faceQueryA.pFace->plane;
			PlaneToWorld(drawPlane, glm::vec3(0), rotationA.value, scaleA);
			debugDraw.DrawPlane(positionA.value, drawPlane, 1.5f, 1.5f, { 1, 0, 0 });
		}
		else
		{
			gmath::Plane drawPlane = faceQueryB.pFace->plane;
			PlaneToWorld(drawPlane, glm::vec3(0), rotationB.value, scaleB);
			debugDraw.DrawPlane(positionB.value, drawPlane, 1.5f, 1.5f, { 0, 0, 1 });
		}
#endif // SAT_DEBUG

		if (aIsBiggerThanB)
		{
			manifold.seperation = faceQueryA.seperation;
			manifold.normal = rotationA.value * faceQueryA.pFace->plane.normal;

			CreateFaceContacts(faceQueryA, positionA.value, rotationA.value, scaleA,
				*hullB.pHull, positionB.value, rotationB.value, scaleB, manifold);
		}
		else
		{
			manifold.seperation = faceQueryB.seperation;
			manifold.normal = rotationB.value * -faceQueryB.pFace->plane.normal;

			CreateFaceContacts(faceQueryB, positionB.value, rotationB.value, scaleB,
				*hullA.pHull, positionA.value, rotationA.value, scaleA, manifold);
		}
	}
	else
	{
		manifold.normal = edgeQuery.normal;
		manifold.seperation = edgeQuery.seperation;
		CreateEdgeContacts(edgeQuery, positionA.value, rotationA.value, scaleA, positionB.value, rotationB.value, scaleB, manifold);

#ifdef SAT_DEBUG
		gmath::Plane drawPlane;
		drawPlane.normal = edgeQuery.normal;
		glm::vec3 edgePos = rotationA.value * (edgeQuery.pEdgeA->pHalfA->pTail->position * scaleA);
		drawPlane.dist = glm::dot(edgePos, edgeQuery.normal);

		debugDraw.DrawPlane(positionA.value, drawPlane, 1.5f, 1.5f, {0, 1, 1});
#endif // SAT_DEBUG

	}

	return true;
}