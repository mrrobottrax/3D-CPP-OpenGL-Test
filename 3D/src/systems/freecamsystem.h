#pragma once

#include <systems/system.h>

class FreecamSystem : public System
{
public:
	FreecamSystem();
	~FreecamSystem();

	void Update() override;
};