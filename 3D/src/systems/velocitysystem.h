#pragma once

#include <systems/system.h>

class VelocitySystem : public System
{
public:
	VelocitySystem();
	~VelocitySystem();

	void Update() override;

	static void UpdatePositions(const std::vector<ChunkArchetypeElement*>* archetypes, const float& deltaTime, bool scaled);
};