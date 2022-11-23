#pragma once

#include <system.h>

class EntitySystem : public System
{
public:
	EntitySystem();
	~EntitySystem();

	void update() override;

private:

};