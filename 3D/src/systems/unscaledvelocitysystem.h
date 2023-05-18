#pragma once
#include <systems/system.h>

class UnscaledVelocitySystem : public System
{
public:
	UnscaledVelocitySystem() {};
	~UnscaledVelocitySystem() {};

	void Update() override;
};