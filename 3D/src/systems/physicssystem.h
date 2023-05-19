#pragma once

#include <systems/system.h>
#include <memory/entity.h>
#include <components/rigidbodycomponent.h>
#include <physics/convexhull.h>

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
	void* pFeatureA = nullptr;
	void* pFeatureB = nullptr;
};

struct ContactPoint
{
	glm::vec3 position = glm::vec3(INFINITY);

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
	void* pRigidBodyA;
	void* pRigidBodyB;

	Entity entityA;
	Entity entityB;
};

struct Manifold
{
	Entity entityA;
	Entity entityB;

	float frictionCoefficient = 0;

	unsigned short numContacts = 0;
	ContactPoint contacts[4];

	float seperation = 0;

	glm::vec3 normal = glm::vec3(0);
	glm::vec3 friction1 = glm::vec3(0);
	glm::vec3 friction2 = glm::vec3(0);
};

#ifdef DEBUG

#define PHYS_DEBUG

#ifdef PHYS_DEBUG

//#define SAT_DEBUG
//#define CONTACT_DEBUG
#define FRICTION_DEBUG

#endif // PHYS_DEBUG
#endif // DEBUG

const int numIterations = 4;
const float gravity = -9.81f;
const float slop = 0.01f;

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Update() override;


	bool HullVsHull(Entity& entityA, Entity& entityB, Manifold& manifold);
};