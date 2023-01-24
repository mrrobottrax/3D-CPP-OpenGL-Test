#pragma once

#include <memory/chunk.h>
#include <memory/chunkarchetypeelement.h>
#include <memory/entity.h>
#include <memory/entitymanager.h>
#include <components/freecamcomponent.h>
#include <components/velocitycomponent.h>
#include <components/rotationcomponent.h>

#include <timemanager.h>

class System
{
public:
	virtual ~System();
	virtual void Update() = 0;

private:

};