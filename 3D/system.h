#pragma once

#include <forward_list>
#include <chunk.h>
#include <chunkArchetypeElement.h>
#include <entity.h>
#include <entityManager.h>
#include <freecamComponent.h>
#include <velocityComponent.h>
#include <timeManager.h>

class System
{
public:
	virtual ~System();
	virtual void update() = 0;

private:

};