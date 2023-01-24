#pragma once

#include <engine/memory/chunk.h>
#include <engine/memory/chunkArchetypeElement.h>
#include <engine/memory/entity.h>
#include <engine/memory/entityManager.h>
#include <engine/freecamComponent.h>
#include <engine/velocityComponent.h>
#include <engine/rotationComponent.h>

#include <engine/timeManager.h>

class System
{
public:
	virtual ~System();
	virtual void update() = 0;

private:

};