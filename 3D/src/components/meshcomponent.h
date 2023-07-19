#pragma once

#include <meshobject.h>

struct MeshComponent
{
	MeshObject* pMesh;

	MeshComponent(MeshObject* p) : pMesh(p) {}
};