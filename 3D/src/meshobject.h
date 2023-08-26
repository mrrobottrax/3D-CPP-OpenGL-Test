#pragma once

#include <gl/glload.h>

typedef GLshort meshIndex_t;
typedef GLsizei meshSize_t;

struct Vertex
{
	float position[3];
	float normal[3];
};

struct MeshObject
{
	GLuint vertexBufferObject;
	GLuint elementBufferObject;

	meshSize_t vertCount;
	meshSize_t indicesCount;
	
	Vertex* vertices;
	meshIndex_t* indices;

	~MeshObject();
	MeshObject() : vertCount(0), indicesCount(0),
		indices(nullptr), vertices(nullptr),
		vertexBufferObject(0), elementBufferObject(0) {};

	void GenBuffers();
	void ClearBuffers();
	void ClearArrays();
};