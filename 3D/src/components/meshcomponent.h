#pragma once

#include <meshObject.h>

struct MeshComponent
{
	MeshObject* pMesh;

	MeshComponent(MeshObject* p) : pMesh(p) {}
};