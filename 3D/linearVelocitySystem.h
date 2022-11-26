#pragma once

#include <system.h>

class LinearVelocitySystem : public System
{
public:
	LinearVelocitySystem();
	~LinearVelocitySystem();

	void update() override;

private:

};