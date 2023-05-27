#pragma once

#include <systems/system.h>
#include <memory/entity.h>
#include <components/rigidbodycomponent.h>
#include <physics/convexhull.h>
#include <map>

struct FaceQuery
{
	float seperation = -FLT_MAX;
	qhFace* pFace = nullptr;
};

struct EdgeQuery
{
	float seperation = -FLT_MAX;
	qhEdge* pEdgeA = nullptr;
	qhEdge* pEdgeB = nullptr;

	glm::vec3 normal = glm::vec3(0);
};

struct FeaturePair
{
	void const* pFeatureA = nullptr;
	void const* pFeatureB = nullptr;

	inline bool operator == (const FeaturePair other) const
	{
		if (pFeatureA == other.pFeatureA && pFeatureB == other.pFeatureB)
			return true;

		return false;
	};
};

struct ContactPoint
{
	glm::vec3 position = glm::vec3(INFINITY);
	float seperation = 0;
	float bias = 0;

	FeaturePair featurePair;

	float inverseEffectiveMassNormal = 0;
	float totalImpulseNormal = 0;
	glm::vec3 crossANormal = glm::vec3(0);
	glm::vec3 crossBNormal = glm::vec3(0);

	float inverseEffectiveMassFriction1 = 0;
	float totalImpulseFriction1 = 0;
	glm::vec3 crossAFriction1 = glm::vec3(0);
	glm::vec3 crossBFriction1 = glm::vec3(0);

	float inverseEffectiveMassFriction2 = 0;
	float totalImpulseFriction2 = 0;
	glm::vec3 crossAFriction2 = glm::vec3(0);
	glm::vec3 crossBFriction2 = glm::vec3(0);
};

struct CollisionPair
{
	Entity entityA;
	Entity entityB;

	CollisionPair(const Entity& entityA, const Entity& entityB)
	{
		if (&entityA < &entityB)
		{
			this->entityA = entityA;
			this->entityB = entityB;
		}
		else
		{
			this->entityA = entityB;
			this->entityB = entityA;
		}
	};

	inline bool operator < (const CollisionPair& other) const
	{
		if (entityA < other.entityA)
			return true;

		if (entityA == other.entityA && entityB < other.entityB)
			return true;

		return false;
	}
};

struct Manifold
{
	float frictionCoefficient = 0;

	unsigned short numContacts = 0;
	ContactPoint contacts[4];

	float seperation = 0;

	glm::vec3 normal = glm::vec3(0);
	glm::vec3 friction1 = glm::vec3(0);
	glm::vec3 friction2 = glm::vec3(0);

	bool isValid = false;
	bool axisIsFace = false;
	bool faceIsPolyA = false;
	void* featureA = nullptr;
	void* featureB = nullptr;

	void UpdateContacts(const Manifold&);
	void UpdateCollisionData(const Manifold&);
	void PreStep(const CollisionPair&);
};

#ifdef DEBUG

//#define PHYS_DEBUG

#ifdef PHYS_DEBUG

//#define SAT_DEBUG
//#define CONTACT_DEBUG
//#define FRICTION_DEBUG
//#define WARMSTART_DEBUG

#endif // PHYS_DEBUG
#endif // DEBUG

const int numIterations = 6;
const float gravity = -9.81f;
const float slop = 0.01f;
const float offset = 0.0f;
const float correctionPercent = 0.2f;
const float velEpsilonLinear = 0;
const float velEpsilonAngular = 0;

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

private:
	std::map<CollisionPair, Manifold> manifolds;

	void ResolveManifolds();

public:
	void Update() override;
	bool HullVsHull(Entity& entityA, Entity& entityB, Manifold& manifold);
};