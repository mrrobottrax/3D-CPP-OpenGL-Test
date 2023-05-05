#pragma once

#include <systems/system.h>
#include <memory/entity.h>
#include <components/rigidbodycomponent.h>
#include <physics/convexhull.h>

struct FaceQuery
{
	float seperation = -FLT_MAX;
	qhFace* pFace = nullptr;

	gMath::Plane plane;
};

struct EdgeQuery
{
	float seperation = -FLT_MAX;
	qhEdge* pEdgeA = nullptr;
	qhEdge* pEdgeB = nullptr;

	gMath::Plane plane;
};

#ifdef DEBUG

#define PHYS_DEBUG

#endif // DEBUG

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Update() override;

	void HullVsHull(Entity entityA, Entity entityB);
};