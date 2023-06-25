#pragma once

#include <memory/chunk.h>
#include <memory/chunkarchetypeelement.h>
#include <memory/entity.h>
#include <memory/entitymanager.h>

#include <timemanager.h>

class System
{
public:
	virtual ~System() {};

	virtual void Init() {};
	virtual void Update() = 0;
};