#pragma once

#include <system.h>
#include <forward_list>

const int chunkSize = 16384;

class EntitySystem : public System
{
public:
	EntitySystem();
	~EntitySystem();

	void update() override;

private:
	
};