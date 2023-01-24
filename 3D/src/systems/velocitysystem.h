#pragma once

#include <systems/system.h>

class VelocitySystem : public System
{
public:
	VelocitySystem();
	~VelocitySystem();

	void Update() override;

private:

};