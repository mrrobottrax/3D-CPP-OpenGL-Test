#pragma once

#include <meshObject.h>

struct MeshComponent
{
	MeshObject* mesh;

	MeshComponent(MeshObject* p) : mesh(p) {}
};