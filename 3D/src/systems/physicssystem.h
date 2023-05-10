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

struct ContactPoint
{
	glm::vec3 position = glm::vec3(INFINITY);
};

struct Manifold
{
	Entity entityA;
	Entity entityB;

	char numContacts = 0;
	ContactPoint contacts[4];

	glm::vec3 normal = glm::vec3(0);
	float seperation = 0;
};

#ifdef DEBUG

#define PHYS_DEBUG

#endif // DEBUG

const int numIterations = 4;
const float gravity = -9.81f;

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Update() override;

	bool HullVsHull(Entity& entityA, Entity& entityB, Manifold& manifold);
};