#pragma once

#include <meshobject.h>

struct MeshComponent
{
	MeshObject* pMesh = nullptr;

	MeshComponent(MeshObject* p) : pMesh(p) {}
};