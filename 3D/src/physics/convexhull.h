#pragma once

struct Plane
{
	glm::vec3 normal;
	float dist;
};

class ConvexHull
{
public:
	ConvexHull() : faceCount(), faces(), vertCount(), vertices()
	{
	};

	ConvexHull(unsigned short faceCount, Plane* faces, unsigned short vertCount, float* vertices)
	{
		// Faces
		this->faceCount = faceCount;
		this->faces = new Plane[faceCount];
		for (int i = 0; i < faceCount; ++i)
		{
			this->faces[i] = faces[i];
		}

		// Vertices
		this->vertCount = vertCount;
		this->vertices = new float[vertCount * 3u];
		for (int i = 0; i < vertCount * 3; ++i)
		{
			this->vertices[i] = vertices[i];
		}
	};

	~ConvexHull();

public:
	unsigned short faceCount;
	unsigned short vertCount;

	Plane* faces;
	float* vertices;
};