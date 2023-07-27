#pragma once

#include <gl/glload.h>

typedef GLshort meshIndex_t;
typedef GLsizei meshSize_t;

struct MeshObject
{
	GLuint positionBufferObject;
	GLuint normalBufferObject;
	GLuint elementBufferObject;

	meshSize_t vertCount;
	meshSize_t normalCount;
	meshSize_t indicesCount;
	float* verts;
	float* normals;
	meshIndex_t* indices;

	~MeshObject();
	MeshObject() : vertCount(0), normalCount(0), indicesCount(0),
		verts(nullptr), indices(nullptr), normals(nullptr),
		positionBufferObject(0), normalBufferObject(0), elementBufferObject(0) {};

	void GenBuffers();
	void ClearBuffers();
};