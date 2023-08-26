#include "pch.h"
#include "meshobject.h"
#include <gl/glutil.h>

void MeshObject::GenBuffers()
{
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(GLshort), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshObject::ClearArrays()
{
	if (indices)
		delete[] indices;

	if (vertices)
		delete[] vertices;
}

void MeshObject::ClearBuffers()
{
	glDeleteBuffers(1, &vertexBufferObject);
	glDeleteBuffers(1, &elementBufferObject);
}

MeshObject::~MeshObject()
{
	ClearBuffers();
	ClearArrays();
}