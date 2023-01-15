#pragma once

#include <engine/common/floats.h>
#include <engine/gl/gl.h>

struct MeshObject
{
	GLuint positionBufferObject;
	GLuint normalBufferObject;
	GLuint elementBufferObject;

	unsigned short vertCount;
	unsigned short normalCount;
	unsigned short indicesCount;
	float* verts;
	float* normals;
	GLshort* indices;

	~MeshObject();
	MeshObject() : vertCount(0), normalCount(0), indicesCount(0), verts(nullptr), indices(nullptr), normals(nullptr), positionBufferObject(0), normalBufferObject(0), elementBufferObject(0) {};

	void genBuffers();
};