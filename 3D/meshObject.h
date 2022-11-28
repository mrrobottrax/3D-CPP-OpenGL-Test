#pragma once

#include <floats.h>
#include <gl.h>

struct MeshObject
{
	unsigned short vertCount;
	unsigned short indicesCount;
	float* verts;
	GLshort* indices;

	MeshObject(unsigned short _vertCount, unsigned short _indicesCount, float* _verts, GLshort* _indices) : vertCount(_vertCount), indicesCount(_indicesCount), verts(_verts), indices(_indices) {}
};