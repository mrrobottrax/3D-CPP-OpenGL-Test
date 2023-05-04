#pragma once

#include <systems/system.h>
#include <memory/entity.h>
#include <components/rigidbodycomponent.h>

#ifdef DEBUG

//#define PHYS_DEBUG

#endif // DEBUG

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Update() override;

	void HullVsHull(Entity entityA, Entity entityB);
};