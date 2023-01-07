#pragma once

#include <system.h>

class FreecamSystem : public System
{
public:
	FreecamSystem();
	~FreecamSystem();

	void update() override;
};