#pragma once

#include <floats.h>
#include <gl.h>

struct MeshObject
{
	unsigned short vertCount;
	unsigned short normalCount;
	unsigned short indicesCount;
	float* verts;
	float* normals;
	GLshort* indices;

	MeshObject() : vertCount(0), normalCount(0), indicesCount(0), verts(nullptr), indices(nullptr), normals(nullptr) {};
};