#pragma once

#include <system.h>

class VelocitySystem : public System
{
public:
	VelocitySystem();
	~VelocitySystem();

	void update() override;

private:

};