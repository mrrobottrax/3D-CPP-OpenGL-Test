#pragma once

#include <systems/system.h>
#include <memory/entity.h>
#include <components/rigidbodycomponent.h>

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	void Update() override;

	void HullVsHull(Entity entityA, Entity entityB);
};