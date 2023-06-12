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

PhysicsSystem::PhysicsSystem() : manifolds()
{
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::ComputeInertia(const Entity& entity)
{
	const EntityManager& em = entityManager;

	MassComponent& mass = em.GetComponent<MassComponent>(entity);
	const RigidBodyComponent& rb = em.GetComponent<RigidBodyComponent>(entity);

	auto ComputeHullInertia = [&em, &entity, &mass]()
	{
		const HullCollider& hull = em.GetComponent<HullCollider>(entity);

		// TODO:
		mass.inertia = glm::vec3(1 / 24.0f);
		mass.inv_inertia = glm::vec3(24.0f);
	};

	switch (rb.colliderType)
	{
	case ColliderType::Hull:
		ComputeHullInertia();
		break;

	default:
		break;
	}
}

CollisionPair::CollisionPair(const Entity& entityA, const Entity& entityB)
{
	const IdComponent& idA = entityManager.GetComponent<IdComponent>(entityA);
	const IdComponent& idB = entityManager.GetComponent<IdComponent>(entityB);

	if (idA.index < idB.index)
	{
		this->entityA = entityA;
		this->entityB = entityB;
	}
	else
	{
		this->entityA = entityB;
		this->entityB = entityA;
	}
}

void Manifold::UpdateContacts(const Manifold& manifold)
{
	ContactPoint mergedContacts[4];

	for (int i = 0; i < manifold.numContacts; ++i)
	{
		const ContactPoint& newContact = manifold.contacts[i];

		// Find matching contact
		int k = -1;
		for (int j = 0; j < numContacts; ++j)
		{
			const ContactPoint& oldContact = contacts[j];

			if (newContact.featurePair == oldContact.featurePair)
			{
				k = j;
				break;
			}
		}

		if (k > -1)
		{
			ContactPoint& contact = mergedContacts[i];
			const ContactPoint& old = contacts[k];

			contact = newContact;

			const float warmStartPercent = 1.0f;
			contact.totalImpulseNormal = old.totalImpulseNormal * warmStartPercent;
			contact.totalImpulseFriction1 = old.totalImpulseFriction1 * warmStartPercent;
			contact.totalImpulseFriction2 = old.totalImpulseFriction2 * warmStartPercent;

#ifdef WARMSTART_DEBUG
			debugDraw.DrawLine(contact.position, contact.position + manifold.normal * contact.totalImpulseNormal, {1, 1, 0}, timeManager.GetFixedDeltaTime());
#endif // WARMSTART_DEBUG

		}
		else
		{
			mergedContacts[i] = newContact;
		}
	}

	*this = manifold;

	for (int i = 0; i < manifold.numContacts; ++i)
		contacts[i] = mergedContacts[i];

	numContacts = manifold.numContacts;
}

void Manifold::UpdateCollisionData(const Manifold& oldManifold)
{
	isValid = true;
	seperation = oldManifold.seperation;
	axisIsFace = oldManifold.axisIsFace;
	featureA = oldManifold.featureA;
	featureB = oldManifold.featureB;
	faceIsPolyA = oldManifold.faceIsPolyA;
}

void ComputeWorldInverseInertiaTensor(const glm::fquat& rotation, const glm::vec3& localInverseInertia, glm::mat3& inertiaTensor)
{
	// No clue what this does but it seems important

	glm::mat3 orientation = glm::mat3(rotation);

	inertiaTensor[0][0] = orientation[0][0] * localInverseInertia.x;
	inertiaTensor[0][1] = orientation[1][0] * localInverseInertia.y;
	inertiaTensor[0][2] = orientation[2][0] * localInverseInertia.z;

	inertiaTensor[1][0] = orientation[0][1] * localInverseInertia.x;
	inertiaTensor[1][1] = orientation[1][1] * localInverseInertia.y;
	inertiaTensor[1][2] = orientation[2][1] * localInverseInertia.z;

	inertiaTensor[2][0] = orientation[0][2] * localInverseInertia.x;
	inertiaTensor[2][1] = orientation[1][2] * localInverseInertia.y;
	inertiaTensor[2][2] = orientation[2][2] * localInverseInertia.z;

	inertiaTensor = orientation * inertiaTensor;
}

void Manifold::PreStep(const CollisionPair& pair)
{
	const EntityManager& em = entityManager;

	const Entity& entityA = pair.entityA;
	const Entity& entityB = pair.entityB;

	const RigidBodyComponent& rbA = em.GetComponent<RigidBodyComponent>(entityA);
	const RigidBodyComponent& rbB = em.GetComponent<RigidBodyComponent>(entityB);

	const PositionComponent& positionA = em.GetComponent<PositionComponent>(entityA);
	const PositionComponent& positionB = em.GetComponent<PositionComponent>(entityB);

	const RotationComponent& rotationA = em.GetComponent<RotationComponent>(entityA);
	const RotationComponent& rotationB = em.GetComponent<RotationComponent>(entityB);

	MassComponent& massComponentA = em.GetComponent<MassComponent>(entityA);
	MassComponent& massComponentB = em.GetComponent<MassComponent>(entityB);

	const float& inv_massA = massComponentA.inv_mass;
	const glm::vec3 localInertiaA = massComponentA.inv_inertia * inv_massA;

	const float& inv_massB = massComponentB.inv_mass;
	const glm::vec3 localInertiaB = massComponentB.inv_inertia * inv_massB;

	inv_inertiaA = glm::mat3(0);
	ComputeWorldInverseInertiaTensor(rotationA.value, localInertiaA, inv_inertiaA);

	inv_inertiaB = glm::mat3(0);
	ComputeWorldInverseInertiaTensor(rotationB.value, localInertiaB, inv_inertiaB);

	VelocityComponent& velocityA = em.GetComponent<VelocityComponent>(entityA);
	VelocityComponent& velocityB = em.GetComponent<VelocityComponent>(entityB);

	// Coefficient of friction
	if (rbA.frictionCombine == Min || rbB.frictionCombine == Min)
	{
		frictionCoefficient = std::fminf(rbA.frictionCoefficient, rbB.frictionCoefficient);
	}
	else if (rbA.frictionCombine == Max || rbB.frictionCombine == Max)
	{
		frictionCoefficient = std::fmaxf(rbA.frictionCoefficient, rbB.frictionCoefficient);
	}
	else
	{
		frictionCoefficient = std::sqrtf(rbA.frictionCoefficient * rbB.frictionCoefficient);
	}

	for (int i = 0; i < numContacts; ++i)
	{
		ContactPoint& contact = contacts[i];

		contact.bias = -correctionPercent / timeManager.GetFixedDeltaTime() * std::fminf(0.0f, contact.seperation + slop);

		contact.crossANormal = glm::cross(contact.position - positionA.value, normal);
		contact.crossBNormal = glm::cross(contact.position - positionB.value, normal);
		contact.crossAFriction1 = glm::cross(contact.position - positionA.value, friction1);
		contact.crossBFriction1 = glm::cross(contact.position - positionB.value, friction1);
		contact.crossAFriction2 = glm::cross(contact.position - positionA.value, friction2);
		contact.crossBFriction2 = glm::cross(contact.position - positionB.value, friction2);

		// Calculate relative mass
		{
			const float n = glm::dot(normal, normal);
			const glm::vec3& cA = contact.crossANormal;
			const glm::vec3& cB = contact.crossBNormal;
			contact.inverseEffectiveMassNormal = 1.0f / ((n * inv_massA) + (n * inv_massB) +
				glm::dot(inv_inertiaA * cA, cA) + glm::dot(inv_inertiaB * cB, cB));

			const float n1 = glm::dot(friction1, friction1);
			const glm::vec3& cA1 = contact.crossAFriction1;
			const glm::vec3& cB1 = contact.crossBFriction1;
			contact.inverseEffectiveMassFriction1 = 1.0f / ((n1 * inv_massA) + (n1 * inv_massB) +
				glm::dot(inv_inertiaA * cA1, cA1) + glm::dot(inv_inertiaB * cB1, cB1));

			const float n2 = glm::dot(friction2, friction2);
			const glm::vec3& cA2 = contact.crossAFriction2;
			const glm::vec3& cB2 = contact.crossBFriction2;
			contact.inverseEffectiveMassFriction2 = 1.0f / ((n2 * inv_massA) + (n2 * inv_massB) +
				glm::dot(inv_inertiaA * cA2, cA2) + glm::dot(inv_inertiaB * cB2, cB2));
		}

		// Apply impulses
		{
			glm::vec3 impulseAL = normal * contact.totalImpulseNormal;
			glm::vec3 impulseAA = contact.crossANormal * contact.totalImpulseNormal;

			glm::vec3 impulseBL = normal * contact.totalImpulseNormal;
			glm::vec3 impulseBA = contact.crossBNormal * contact.totalImpulseNormal;

			impulseAL += (friction1 * contact.totalImpulseFriction1) + (friction2 * contact.totalImpulseFriction2);
			impulseAA += (contact.crossAFriction1 * contact.totalImpulseFriction1) + (contact.crossAFriction2 * contact.totalImpulseFriction2);

			impulseBL += (friction1 * contact.totalImpulseFriction1) + (friction2 * contact.totalImpulseFriction2);
			impulseBA += (contact.crossBFriction1 * contact.totalImpulseFriction1) + (contact.crossBFriction2 * contact.totalImpulseFriction2);

			velocityA.linear -= inv_massA * impulseAL;
			velocityA.angular -= inv_inertiaA * impulseAA;

			velocityB.linear += inv_massB * impulseBL;
			velocityB.angular += inv_inertiaB * impulseBA;
		}
	}
}

void PhysicsSystem::ResolveManifolds()
{
	const EntityManager& em = entityManager;

	// Precalculate some stuff
	for (auto manifoldIter = manifolds.begin(); manifoldIter != manifolds.end(); ++manifoldIter)
	{
		Manifold& manifold = manifoldIter->second;
		const CollisionPair& pair = manifoldIter->first;

		manifold.PreStep(pair);
	}

	for (int iter = 0; iter < numIterations; ++iter)
	{
		for (auto manifoldIter = manifolds.begin(); manifoldIter != manifolds.end(); ++manifoldIter)
		{
			Manifold& manifold = manifoldIter->second;
			const CollisionPair& pair = manifoldIter->first;

#ifdef CONTACT_DEBUG
			for (int i = 0; i < manifold.numContacts; ++i)
			{
				debugDraw.DrawLine(manifold.contacts[i].position, manifold.contacts[i].position + manifold.normal * 0.2f, { 1, 0, 1 }, timeManager.GetFixedDeltaTime());
			}
#endif // CONTACT_DEBUG

			const Entity& entityA = pair.entityA;
			const Entity& entityB = pair.entityB;

			const RigidBodyComponent& rbA = em.GetComponent<RigidBodyComponent>(entityA);
			const RigidBodyComponent& rbB = em.GetComponent<RigidBodyComponent>(entityB);

			PositionComponent& positionA = em.GetComponent<PositionComponent>(entityA);
			PositionComponent& positionB = em.GetComponent<PositionComponent>(entityB);
			RotationComponent& rotationA = em.GetComponent<RotationComponent>(entityA);
			RotationComponent& rotationB = em.GetComponent<RotationComponent>(entityB);

			const float& inv_massA = em.GetComponent<MassComponent>(entityA).inv_mass;
			const float& inv_massB = em.GetComponent<MassComponent>(entityB).inv_mass;

			VelocityComponent& velocityA = em.GetComponent<VelocityComponent>(entityA);
			VelocityComponent& velocityB = em.GetComponent<VelocityComponent>(entityB);

			const glm::vec3& normal = manifold.normal;
			const glm::vec3& friction1 = manifold.friction1;
			const glm::vec3& friction2 = manifold.friction2;

			const glm::vec3& vA = velocityA.linear;
			const glm::vec3& vB = velocityB.linear;
			const glm::vec3& wA = velocityA.angular;
			const glm::vec3& wB = velocityB.angular;

			for (int i = 0; i < manifold.numContacts; ++i)
			{
				ContactPoint& contact = manifold.contacts[i];

				if (contact.seperation > 0)
					continue;

				glm::vec3 impulseAL = glm::vec3(0);
				glm::vec3 impulseAA = glm::vec3(0);
				glm::vec3 impulseBL = glm::vec3(0);
				glm::vec3 impulseBA = glm::vec3(0);

				// Normal Force
				{
					const float oldImpulse = contact.totalImpulseNormal;

					const float velocityAtPoint = glm::dot(-normal, vA) + glm::dot(-contact.crossANormal, wA)
						+ glm::dot(normal, vB) + glm::dot(contact.crossBNormal, wB);
					contact.totalImpulseNormal += contact.inverseEffectiveMassNormal * (-velocityAtPoint + contact.bias);

					// Enforce Signorini conditions
					if (contact.totalImpulseNormal < 0)
						contact.totalImpulseNormal = 0;

					const float deltaImpulse = contact.totalImpulseNormal - oldImpulse;

					impulseAL += normal * deltaImpulse;
					impulseAA += contact.crossANormal * deltaImpulse;

					impulseBL += normal * deltaImpulse;
					impulseBA += contact.crossBNormal * deltaImpulse;
				}

				// Friction
				// TODO: Only do friction at the center of the manifold
				{
					const float oldImpulse1 = contact.totalImpulseFriction1;
					const float oldImpulse2 = contact.totalImpulseFriction2;

					const float velocityAtFriction1 = glm::dot(-friction1, vA) + glm::dot(-contact.crossAFriction1, wA)
						+ glm::dot(friction1, vB) + glm::dot(contact.crossBFriction1, wB);
					const float velocityAtFriction2 = glm::dot(-friction2, vA) + glm::dot(-contact.crossAFriction2, wA)
						+ glm::dot(friction2, vB) + glm::dot(contact.crossBFriction2, wB);

					contact.totalImpulseFriction1 += -contact.inverseEffectiveMassFriction1 * velocityAtFriction1;
					contact.totalImpulseFriction2 += -contact.inverseEffectiveMassFriction2 * velocityAtFriction2;

					// Clamp force
					// This is a friction pyramid which is slightly innacurate, but it's faster
					const float& frictionC = manifold.frictionCoefficient;

					contact.totalImpulseFriction1 = clamp(contact.totalImpulseFriction1,
						-contact.totalImpulseNormal * frictionC, contact.totalImpulseNormal * frictionC);

					contact.totalImpulseFriction2 = clamp(contact.totalImpulseFriction2,
						-contact.totalImpulseNormal * frictionC, contact.totalImpulseNormal * frictionC);

					const float deltaImpulse1 = contact.totalImpulseFriction1 - oldImpulse1;
					const float deltaImpulse2 = contact.totalImpulseFriction2 - oldImpulse2;

					impulseAL += (friction1 * deltaImpulse1) + (friction2 * deltaImpulse2);
					impulseAA += (contact.crossAFriction1 * deltaImpulse1) + (contact.crossAFriction2 * deltaImpulse2);

					impulseBL += (friction1 * deltaImpulse1) + (friction2 * deltaImpulse2);
					impulseBA += (contact.crossBFriction1 * deltaImpulse1) + (contact.crossBFriction2 * deltaImpulse2);

#ifdef FRICTION_DEBUG
					{
						const glm::vec3& pos = manifold.contacts[i].position;

						debugDraw.DrawLine(pos, pos + friction1 * velocityAtFriction1, { 1, 0, 0 }, timeManager.GetFixedDeltaTime());
						debugDraw.DrawLine(pos, pos + friction2 * velocityAtFriction2, { 0, 0, 1 }, timeManager.GetFixedDeltaTime());
						debugDraw.DrawLine(pos, pos + normal * 0.2f, { 0, 1, 0 }, timeManager.GetFixedDeltaTime());

						debugDraw.DrawLine(pos, pos + friction1 * deltaImpulse1, { 1, 0, 0 }, timeManager.GetFixedDeltaTime());
						debugDraw.DrawLine(pos, pos + friction2 * deltaImpulse2, { 0, 0, 1 }, timeManager.GetFixedDeltaTime());
						debugDraw.DrawLine(pos, pos + normal * 0.2f, { 0, 1, 0 }, timeManager.GetFixedDeltaTime());
					}
#endif // FRICTION_DEBUG

				}

				glm::vec3& velLinearA = velocityA.linear;
				glm::vec3& velAngularA = velocityA.angular;
				glm::vec3& velLinearB = velocityB.linear;
				glm::vec3& velAngularB = velocityB.angular;

				velLinearA -= inv_massA * impulseAL;
				velAngularA -= manifold.inv_inertiaA * impulseAA;

				velLinearB += inv_massB * impulseBL;
				velAngularB += manifold.inv_inertiaB * impulseBA;
			}
		}
	}

	// Stop bodies with low velocity
	// TODO: This should probably be energy based or something
	for (auto manifoldIter = manifolds.begin(); manifoldIter != manifolds.end(); ++manifoldIter)
	{
		Manifold& manifold = manifoldIter->second;
		const CollisionPair& pair = manifoldIter->first;

		const Entity& entityA = pair.entityA;
		const Entity& entityB = pair.entityB;

		VelocityComponent& velocityA = em.GetComponent<VelocityComponent>(entityA);
		VelocityComponent& velocityB = em.GetComponent<VelocityComponent>(entityB);

		RigidBodyComponent& rbA = em.GetComponent<RigidBodyComponent>(entityA);
		RigidBodyComponent& rbB = em.GetComponent<RigidBodyComponent>(entityB);

		glm::vec3& velLinearA = velocityA.linear;
		glm::vec3& velAngularA = velocityA.angular;
		glm::vec3& velLinearB = velocityB.linear;
		glm::vec3& velAngularB = velocityB.angular;

		RigidBodyComponent* rbs[2] = { &rbA, &rbB };
		glm::vec3* linearVels[2] = { &velLinearA, &velLinearB };
		glm::vec3* angularVels[2] = { &velAngularA, &velAngularB };

		for (int i = 0; i < 2; ++i)
		{
			glm::vec3& linearVel = *linearVels[i];
			glm::vec3& angularVel = *angularVels[i];

			if (glm::dot(linearVel, linearVel) <= velEpsilonLinear &&
				glm::dot(angularVel, angularVel) <= velEpsilonAngular)
			{
				linearVel = glm::vec3(0);
				angularVel = glm::vec3(0);
				rbs[i]->sleeping = true;
			}
			else
			{
				rbs[i]->sleeping = false;
			}
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
				vel.linear.y += rb.gravityScale * gravity * timeManager.GetFixedDeltaTime();
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

							const MassComponent& massA = em.GetComponent<MassComponent>(entity);
							const MassComponent& massB = em.GetComponent<MassComponent>(entity2);

							// No need to check for collisions when both are static
							if ((rb.isStatic || rb.sleeping) && (rb2.isStatic || rb2.sleeping))
								continue;

							// Unstoppable force vs immovable object
							if (massA.inv_mass == 0 && massB.inv_mass == 0)
								continue;

							// Don't allow zero mass/inertia
							if (massA.mass == 0 || glm::dot(massA.inertia, massA.inertia) == 0 ||
								massB.mass == 0 || glm::dot(massB.inertia, massB.inertia) == 0)
								continue;

							// TODO: Test against AABB tree

							CollisionPair pair(entity, entity2);
							pairs.push_back(pair);
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

		RigidBodyComponent& rbA = entityManager.GetComponent<RigidBodyComponent>(entityA);
		RigidBodyComponent& rbB = entityManager.GetComponent<RigidBodyComponent>(entityB);

		Manifold manifold;

		// Get last frame collision data
		{
			auto iter = manifolds.find(*pairsIt);
			if (iter != manifolds.end())
			{
				manifold.UpdateCollisionData(iter->second);
			}
		}

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
			auto iter = manifolds.find(*pairsIt);
			if (iter == manifolds.end())
			{
				manifolds.insert(std::pair<CollisionPair, Manifold>(*pairsIt, manifold));
			}
			else
			{
				iter->second.UpdateContacts(manifold);
			}
		}
		else
		{
			manifolds.erase(*pairsIt);
		}
	}

	ResolveManifolds();
}

// Find vertex with most penetration
// Plane MUST be in hull space
float GetSeperationDepth(gmath::Plane testPlane, const ConvexHull& hull, const glm::vec3& scale)
{

#ifdef SAT_DEBUG
	debugDraw.DrawPlane(glm::vec3(0), testPlane, 1.5f, 1.5f, { 1, 0, 0 }, timeManager.GetFixedDeltaTime());
#endif // SAT_DEBUG

	testPlane.normal *= scale;

	float minSeperation = FLT_MAX;

	// TODO: Find support point by walking up mesh
	for (int v = 0; v < hull.vertCount; ++v)
	{
		glm::vec3 point = hull.verts[v].position;

		float dot = glm::dot(point, testPlane.normal);

		if (dot <= minSeperation)
		{
			minSeperation = dot;
		}

#ifdef SAT_DEBUG
		debugDraw.DrawLine(point, point + glm::vec3(0, 0.1f, 0), { 0, 0, 1 }, timeManager.GetFixedDeltaTime());
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

float GetSingleFaceSeperation(const qhFace& face, const glm::vec3& positionA, const glm::fquat& rotationA, const glm::vec3& scaleA,
	const HullCollider& hullB, const glm::vec3& positionB, const glm::fquat& rotationB, const glm::vec3& scaleB)
{
	gmath::Plane testPlane = face.plane;

	ScalePlane(testPlane, scaleA);
	PlaneToWorld(testPlane, positionA, rotationA);
	PlaneToSpace(testPlane, positionB, rotationB);

#ifdef SAT_DEBUG
	Plane drawPlane = testPlane;
	PlaneToWorld(drawPlane, positionB, rotationB);
	PlaneToSpace(drawPlane, positionA, rotationA);
	PlaneToWorld(drawPlane, glm::vec3(0), rotationA);
	debugDraw.DrawPlane(positionA, drawPlane, 1.5f, 1.5f, glm::vec3(0.5f), timeManager.GetFixedDeltaTime());
#endif // SAT_DEBUG

	return GetSeperationDepth(testPlane, *hullB.pHull, scaleB);
}

FaceQuery SatFaceTest(const HullCollider& hullA, const glm::vec3& positionA, const glm::fquat& rotationA, const glm::vec3& scaleA,
	const HullCollider& hullB, const glm::vec3& positionB, const glm::fquat& rotationB, const glm::vec3& scaleB)
{
	FaceQuery query = FaceQuery();
	query.seperation = -FLT_MAX;

	for (int f = 0; f < hullA.pHull->faceCount; ++f)
	{
		const float& seperation = GetSingleFaceSeperation(hullA.pHull->faces[f], positionA, rotationA, scaleA,
			hullB, positionB, rotationB, scaleB);

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
	debugDraw.DrawPlane(positionA, drawPlane, 1.5f, 1.5f, { 1, 0.5f, 0 }, timeManager.GetFixedDeltaTime());
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
			debugDraw.DrawPlane(positionA, drawPlane, 0.2f, 0.2f, { 0, 0.5f, 0 }, timeManager.GetFixedDeltaTime());
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

	if (t < 0 || t > 1)
	{
		return glm::vec3(NAN);
	}

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

	const glm::vec3 pointA = GetClosestPointOnLine(lineA, lineB);
	const glm::vec3 pointB = GetClosestPointOnLine(lineB, lineA);

	// Line don't intersect
	if (glm::any(glm::isnan(pointA)) || glm::any(glm::isnan(pointB)))
	{
		manifold.numContacts = 0;
		return;
	}

	// Calculate line directions
	glm::vec3 dirA = lineA.pointB - lineA.pointA;
	manifold.friction1 = dirA;
	manifold.friction2 = glm::cross(dirA, manifold.normal);
	manifold.friction1 = glm::normalize(manifold.friction1);
	manifold.friction2 = glm::normalize(manifold.friction2);

#ifdef CONTACT_DEBUG
	debugDraw.DrawLine(lineA.pointA, lineA.pointB, { 1, 1, 0 }, timeManager.GetFixedDeltaTime());
	debugDraw.DrawLine(lineB.pointA, lineB.pointB, { 1, 1, 0 }, timeManager.GetFixedDeltaTime());
#endif // CONTACT_DEBUG

	glm::vec3 average = (pointA - pointB) / 2.0f + pointB;

	ContactPoint contact;
	contact.position = average;
	contact.featurePair = {
		query.pEdgeA,
		query.pEdgeB
	};
	contact.seperation = query.seperation;

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
	manifold.friction1 = rotationA * (scaleA * queryA.pFace->pEdge->pTail->position - scaleB * queryA.pFace->pEdge->pTwin->pTail->position);
	manifold.friction2 = glm::cross(manifold.friction1, manifold.normal);
	manifold.friction1 = glm::normalize(manifold.friction1);
	manifold.friction2 = glm::normalize(manifold.friction2);

	struct SimpleContact
	{
		glm::vec3 pos;
		FeaturePair featurePair;
		float seperation = 0.0f;
	};

	// Clip with Sutherland-Hodgman
	std::vector<SimpleContact> in;
	std::vector<SimpleContact> out;

	// Get starting verts
	{
		const qhHalfEdge* startEdge = incidentFace.pEdge;
		qhHalfEdge* pEdge = const_cast<qhHalfEdge*>(startEdge);
		do
		{
			in.push_back({
				pEdge->pTail->position * scaleB,
				{ pEdge, nullptr }
				});

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
			auto& inBuffer = swapBuffers ? out : in;
			auto& outBuffer = !swapBuffers ? out : in;

			const qhFace& clipFace = *pEdge->pTwin->pFace;
			Plane clipPlane = clipFace.plane;
			ScalePlane(clipPlane, scaleA);

			PlaneToWorld(clipPlane, positionA, rotationA);
			PlaneToSpace(clipPlane, positionB, rotationB);

#ifdef CONTACT_DEBUG
			Plane drawPlane = clipPlane;
			PlaneToWorld(drawPlane, glm::vec3(0), rotationB);
			debugDraw.DrawPlane(positionB, drawPlane, 1.5f, 1.5f, { 1, 1, 1 }, timeManager.GetFixedDeltaTime());
#endif // CONTACT_DEBUG

			// Keep vertices behind plane
			for (int i = 0; i < inBuffer.size(); ++i)
			{
				const glm::vec3& point = inBuffer[i].pos;
				const float dist = glm::dot(point, clipPlane.normal) - clipPlane.dist;

#ifdef CONTACT_DEBUG
				glm::vec3 drawPoint = rotationB * point + positionB;
				debugDraw.DrawLine(drawPoint, drawPoint + manifold.normal * 0.2f, { 0, 0.2f, 0 }, timeManager.GetFixedDeltaTime());
#endif // CONTACT_DEBUG

				const bool pointBehindPlane = dist <= 0;

				const int j = i - 1;
				const glm::vec3& prev = inBuffer[j < 0 ? inBuffer.size() - 1 : j].pos;

				const bool lineCrossesPlane = (glm::dot(prev, clipPlane.normal) - clipPlane.dist <= 0) != pointBehindPlane;
				if (lineCrossesPlane)
				{
					// Find intersection of line and plane
					const Line line = {
						prev,
						point
					};

					const glm::vec3 point = LineAndPlane(line, clipPlane);
					outBuffer.push_back({
						point,
						{ inBuffer[i].featurePair.pFeatureA, &clipFace }
						});

#ifdef CONTACT_DEBUG
					debugDraw.DrawPlane(glm::vec3(0), clipPlane, 1.5f, 1.5f, {1, 1, 0}, timeManager.GetFixedDeltaTime());
					debugDraw.DrawLine(line.pointA, line.pointB, { 1, 1, 1 }, timeManager.GetFixedDeltaTime());

					glm::vec3 drawPoint = point;
					drawPoint = rotationB * drawPoint + positionB;
					debugDraw.DrawLine(drawPoint, drawPoint + glm::vec3(0, 0.2f, 0), { 1, 1, 1 }, timeManager.GetFixedDeltaTime());
#endif // CONTACT_DEBUG

				}
				
				if (pointBehindPlane)
				{
					outBuffer.push_back(inBuffer[i]);
				}
			}

			inBuffer.clear();
			swapBuffers = !swapBuffers;

			pEdge = pEdge->pNext;
		} while (pEdge != startEdge);
	}

	// Remove contacts above the reference face
	{
		auto& inBuffer = swapBuffers ? out : in;
		auto& outBuffer = !swapBuffers ? out : in;
		for (int i = 0; i < inBuffer.size(); ++i)
		{
			SimpleContact& contact = inBuffer[i];
			const glm::vec3& point = inBuffer[i].pos;
			const float dist = glm::dot(relativeReferencePlane.normal, point) - relativeReferencePlane.dist;

			// Keep contacts for warm starting even when slightly apart
			if (dist <= 0)
			{
				contact.seperation = dist;
				outBuffer.push_back(inBuffer[i]);
			}
		}

		swapBuffers = !swapBuffers;
		inBuffer.clear();
	}

	// Project onto reference plane
	{
		auto& inBuffer = swapBuffers ? out : in;

		for (int i = 0; i < inBuffer.size(); ++i)
		{
			glm::vec3& point = inBuffer[i].pos;
			const float dist = glm::dot(relativeReferencePlane.normal, point) - relativeReferencePlane.dist;

			point -= relativeReferencePlane.normal * dist;
		}
	}

	// Reduce contact points
	{
		auto& inBuffer = swapBuffers ? out : in;

		if (inBuffer.size() > 4)
		{
			auto& outBuffer = !swapBuffers ? out : in;

			// TODO: First point should be deepest for continuous collision
			int firstPoint = 0;
			{
				const glm::vec3 direction = glm::vec3(1, 1, 1);

				float bestDot = -FLT_MAX;
				for (int i = 1; i < inBuffer.size(); ++i)
				{
					const glm::vec3& point = inBuffer[i].pos;
					const float dot = glm::dot(point, direction);

					if (dot >= bestDot)
					{
						firstPoint = i;
						bestDot = dot;
					}
				}
			}

			// Get furthest point
			int secondPoint = -1;
			{
				float bestDist = -FLT_MAX;
				for (int i = 0; i < inBuffer.size(); ++i)
				{
					const glm::vec3& pPoint = inBuffer[i].pos;

					if (&pPoint == &inBuffer[firstPoint].pos)
						continue;

					const glm::vec3 tmp = pPoint - inBuffer[firstPoint].pos;
					const float sqrDist = SqrDist(pPoint, inBuffer[firstPoint].pos);

					if (sqrDist >= bestDist)
					{
						secondPoint = i;
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
			int thirdPoint = -1;
			bool thirdSign = 0;
			{
				float bestArea = 0;
				for (int i = 0; i < inBuffer.size(); ++i)
				{
					const glm::vec3& pPoint = inBuffer[i].pos;

					if (&pPoint == &inBuffer[firstPoint].pos)
						continue;

					if (&pPoint == &inBuffer[secondPoint].pos)
						continue;

					assert(secondPoint > -1);

					const float area = GetArea(inBuffer[firstPoint].pos, inBuffer[secondPoint].pos, pPoint);

					if (abs(area) >= abs(bestArea))
					{
						thirdPoint = i;
						bestArea = area;
						thirdSign = signbit(area);
					}
				}
			}

			// Get fourth point
			int fourthPoint = -1;
			bool fourthSign = 0;
			{
				float bestArea = 0;
				for (int i = 0; i < inBuffer.size(); ++i)
				{
					const glm::vec3& pPoint = inBuffer[i].pos;

					if (&pPoint == &inBuffer[firstPoint].pos)
						continue;

					if (&pPoint == &inBuffer[secondPoint].pos)
						continue;

					assert(secondPoint > -1);

					const float area = GetArea(inBuffer[firstPoint].pos, inBuffer[secondPoint].pos, pPoint);

					bool sign = signbit(area);
					if (abs(area) >= abs(bestArea) && sign != thirdSign)
					{
						fourthPoint = i;
						bestArea = area;
						fourthSign = sign;
					}
				}
			}

			assert(firstPoint > -1);
			assert(secondPoint > -1);
			assert(thirdPoint > -1);

			outBuffer.push_back(inBuffer[firstPoint]);
			outBuffer.push_back(inBuffer[secondPoint]);
			outBuffer.push_back(inBuffer[thirdPoint]);

			if (fourthPoint > -1)
			{
				outBuffer.push_back(inBuffer[fourthPoint]);
			}

			swapBuffers = !swapBuffers;
			inBuffer.clear();
		}
	}

	// Move contacts to world space
	{
		const auto& inBuffer = swapBuffers ? out : in;
		{
			manifold.numContacts = (int)inBuffer.size();
			for (int i = 0; i < inBuffer.size(); ++i)
			{
				manifold.contacts[i].position =	rotationB * inBuffer[i].pos + positionB;
				manifold.contacts[i].featurePair = inBuffer[i].featurePair;
				manifold.contacts[i].seperation = inBuffer[i].seperation;
			}
		}
	}
}

bool PhysicsSystem::HullVsHull(const Entity& entityA, const Entity& entityB, Manifold& manifold)
{
	// Seperating Axis Theorum

	const EntityManager& em = entityManager;

	const PositionComponent& positionA = em.GetComponent<PositionComponent>(entityA);
	const PositionComponent& positionB = em.GetComponent<PositionComponent>(entityB);
	const RotationComponent& rotationA = em.GetComponent<RotationComponent>(entityA);
	const RotationComponent& rotationB = em.GetComponent<RotationComponent>(entityB);

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

			debugDraw.DrawLine(start, end, color, timeManager.GetFixedDeltaTime());
		}
	};

	DrawHull(hullA.pHull, positionA.value, rotationA.value, scaleA, {1, 0, 0});
	DrawHull(hullB.pHull, positionB.value, rotationB.value, scaleB, {0, 0, 1});

#endif // PHYS_DEBUG

	// Temporal coherence
	if (manifold.isValid)
	{
		bool wasColliding = manifold.seperation <= 0;
		bool isColliding = false;

		Manifold newManifold;

		if (manifold.axisIsFace)
		{
			float seperation;
			
			if (manifold.faceIsPolyA)
				seperation = GetSingleFaceSeperation(*static_cast<qhFace*>(manifold.featureA),
					positionA.value, rotationA.value, scaleA,
					hullB, positionB.value, rotationB.value, scaleB);
			else
				seperation = GetSingleFaceSeperation(*static_cast<qhFace*>(manifold.featureA),
					positionB.value, rotationB.value, scaleB,
					hullA, positionA.value, rotationA.value, scaleA);

			isColliding = seperation <= 0;

			if (manifold.faceIsPolyA)
				newManifold.normal = rotationA.value * (*static_cast<qhFace*>(manifold.featureA)).plane.normal;
			else
				newManifold.normal = rotationB.value * (*static_cast<qhFace*>(manifold.featureA)).plane.normal;

			newManifold.axisIsFace = true;
			newManifold.featureA = manifold.featureA;
			newManifold.seperation = seperation;
			newManifold.faceIsPolyA = manifold.faceIsPolyA;
		}
		else
		{
			const qhEdge& edgeA = *static_cast<qhEdge*>(manifold.featureA);
			const qhEdge& edgeB = *static_cast<qhEdge*>(manifold.featureB);

			const glm::vec3& edgeATail = rotationA.value * (edgeA.pHalfA->pTail->position * scaleA) + positionA.value;
			const glm::vec3& edgeADir = rotationA.value * ((edgeA.pHalfB->pTail->position - edgeA.pHalfA->pTail->position) * scaleA);
			const glm::vec3& edgeBDir = rotationB.value * ((edgeB.pHalfB->pTail->position - edgeB.pHalfA->pTail->position) * scaleB);

			if (EdgesBuildMinkowskiFace(edgeA, edgeADir, rotationA.value,
				edgeB, edgeBDir, rotationB.value))
			{
				gmath::Plane testPlane;
				testPlane.normal = glm::normalize(glm::cross(edgeADir, edgeBDir));

				// Check if the normal is facing the right direction
				if (glm::dot(edgeATail - positionA.value, testPlane.normal) < 0)
				{
					testPlane.normal = -testPlane.normal;
				}

				// Move plane to edge
				testPlane.dist = glm::dot(edgeATail, testPlane.normal);

				newManifold.normal = testPlane.normal;

				PlaneToSpace(testPlane, positionB.value, rotationB.value);
				float seperation = GetSeperationDepth(testPlane, *hullB.pHull, scaleB);

				isColliding = seperation <= 0;

				newManifold.axisIsFace = false;
				newManifold.featureA = manifold.featureA;
				newManifold.featureB = manifold.featureB;
				newManifold.seperation = seperation;
			}
		}

		if (wasColliding == isColliding)
		{
			// TODO: Do we even need to calculate if colliding?
			// Maybe just use contact generation
			if (isColliding)
			{
				// Regen contacts
				if (newManifold.axisIsFace)
				{
					// Prev collision was face

					FaceQuery faceQuery;
					faceQuery.pFace = static_cast<qhFace*>(newManifold.featureA);
					faceQuery.seperation = newManifold.seperation;

					if (newManifold.faceIsPolyA)
						CreateFaceContacts(faceQuery, positionA.value, rotationA.value, scaleA,
							*hullB.pHull, positionB.value, rotationB.value, scaleB, newManifold);
					else
						CreateFaceContacts(faceQuery, positionB.value, rotationB.value, scaleB,
							*hullA.pHull, positionA.value, rotationA.value, scaleA, newManifold);

					if (!newManifold.faceIsPolyA)
						newManifold.normal *= -1;
				}
				else
				{
					// Prev collision was edge

					EdgeQuery edgeQuery;
					edgeQuery.pEdgeA = static_cast<qhEdge*>(newManifold.featureA);
					edgeQuery.pEdgeB = static_cast<qhEdge*>(newManifold.featureB);
					edgeQuery.seperation = newManifold.seperation;

					CreateEdgeContacts(edgeQuery, positionA.value, rotationA.value, scaleA,
						positionB.value, rotationB.value, scaleB, newManifold);
				}

				if (newManifold.numContacts > 0)
				{
					manifold = newManifold;
					return true;
				}
			}
			else
			{
				// Found seperating axis
				manifold = newManifold;
				return false;
			}
		}
	}

	// Check faces of A
	FaceQuery faceQueryA = SatFaceTest(hullA, positionA.value, rotationA.value, scaleA, hullB, positionB.value, rotationB.value, scaleB);
	if (faceQueryA.seperation > 0)
	{
		return false;
	}

	// Check faces of B
	FaceQuery faceQueryB = SatFaceTest(hullB, positionB.value, rotationB.value, scaleB, hullA, positionA.value, rotationA.value, scaleA);
	if (faceQueryB.seperation > 0)
	{
		return false;
	}

	// Check edge combinations
	EdgeQuery edgeQuery = SatEdgeTest(hullA, positionA.value, rotationA.value, scaleA, hullB, positionB.value, rotationB.value, scaleB);
	if (edgeQuery.seperation > 0)
	{
		return false;
	}

	// Prefer face collisions
	const float faceBias = 0.01f;
	bool isFaceContactA = faceQueryA.seperation + faceBias >= edgeQuery.seperation;
	bool isFaceContactB = faceQueryB.seperation + faceBias >= edgeQuery.seperation;

	if (isFaceContactA || isFaceContactB)
	{
		// TODO: Only switch when A is significantly larger than B
		bool aIsBiggerThanB = faceQueryA.seperation >= faceQueryB.seperation;

#ifdef SAT_DEBUG
		if (aIsBiggerThanB)
		{
			gmath::Plane drawPlane = faceQueryA.pFace->plane;
			PlaneToWorld(drawPlane, glm::vec3(0), rotationA.value);
			debugDraw.DrawPlane(positionA.value, drawPlane, 1.5f, 1.5f, { 1, 0, 0 }, timeManager.GetFixedDeltaTime());
		}
		else
		{
			gmath::Plane drawPlane = faceQueryB.pFace->plane;
			PlaneToWorld(drawPlane, glm::vec3(0), rotationB.value);
			debugDraw.DrawPlane(positionB.value, drawPlane, 1.5f, 1.5f, { 0, 0, 1 }, timeManager.GetFixedDeltaTime());
		}
#endif // SAT_DEBUG

		if (aIsBiggerThanB)
		{
			manifold.normal = rotationA.value * faceQueryA.pFace->plane.normal;
			manifold.seperation = faceQueryA.seperation;
			manifold.axisIsFace = true;
			manifold.featureA = faceQueryA.pFace;
			manifold.faceIsPolyA = true;

			CreateFaceContacts(faceQueryA, positionA.value, rotationA.value, scaleA,
				*hullB.pHull, positionB.value, rotationB.value, scaleB, manifold);
		}
		else
		{
			manifold.normal = rotationB.value * faceQueryB.pFace->plane.normal;
			manifold.seperation = faceQueryB.seperation;
			manifold.axisIsFace = true;
			manifold.featureA = faceQueryB.pFace;
			manifold.faceIsPolyA = false;

			CreateFaceContacts(faceQueryB, positionB.value, rotationB.value, scaleB,
				*hullA.pHull, positionA.value, rotationA.value, scaleA, manifold);
			manifold.normal *= -1;
		}
	}
	else
	{
		manifold.normal = edgeQuery.normal;
		manifold.seperation = edgeQuery.seperation;
		manifold.axisIsFace = false;
		manifold.featureA = edgeQuery.pEdgeA;
		manifold.featureB = edgeQuery.pEdgeB;

		CreateEdgeContacts(edgeQuery, positionA.value, rotationA.value, scaleA, positionB.value, rotationB.value, scaleB, manifold);

#ifdef SAT_DEBUG
		gmath::Plane drawPlane;
		drawPlane.normal = edgeQuery.normal;
		glm::vec3 edgePos = rotationA.value * (edgeQuery.pEdgeA->pHalfA->pTail->position * scaleA);
		drawPlane.dist = glm::dot(edgePos, edgeQuery.normal);

		debugDraw.DrawPlane(positionA.value, drawPlane, 1.5f, 1.5f, {0, 1, 1}, timeManager.GetFixedDeltaTime());
#endif // SAT_DEBUG

	}

	return true;
}
