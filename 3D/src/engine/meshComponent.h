#pragma once

#include <engine/meshObject.h>

struct MeshComponent
{
	MeshObject* mesh;

	MeshComponent(MeshObject* p) : mesh(p) {}
};