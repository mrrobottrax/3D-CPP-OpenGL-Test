#include "pch.h"
#include "meshobject.h"

void MeshObject::GenBuffers()
{
	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertsSize * sizeof(float), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normalBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, normalsSize * sizeof(float), normals, GL_STATIC_DRAW);
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

	if (verts)
		delete[] verts;

	if (normals)
		delete[] normals;
}

void MeshObject::ClearBuffers()
{
	glDeleteBuffers(1, &positionBufferObject);
	glDeleteBuffers(1, &normalBufferObject);
	glDeleteBuffers(1, &elementBufferObject);
}

MeshObject::~MeshObject()
{
	ClearBuffers();
	ClearArrays();
}