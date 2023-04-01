#include <pch.h>
#include <physics/convexhull.h>

ConvexHull::ConvexHull(unsigned short planeCount, Plane* planes, unsigned short vertCount, float* vertices, unsigned short edgeCount, float* edges)
{
	{
		// Faces
		this->planeCount = planeCount;
		this->planes = new Plane[planeCount];
		for (int i = 0; i < planeCount; ++i)
		{
			this->planes[i] = planes[i];
		}

		// Vertices
		this->vertCount = vertCount;
		this->vertices = new float[vertCount * 3];
		for (int v = 0; v < vertCount; ++v)
		{
			int i = v * 3;

			this->vertices[i] = vertices[i];
			this->vertices[i + 1] = vertices[i + 1];
			this->vertices[i + 2] = vertices[i + 2];
		}

		// Edges
		this->edgeCount = edgeCount;
		this->edges = new float[(int)edgeCount * 6];
		for (int e = 0; e < edgeCount; ++e)
		{
			int i = e * 6;
			this->edges[i] = edges[i];
			this->edges[i + 1] = edges[i + 1];
			this->edges[i + 2] = edges[i + 2];

			// Normalize direction vectors
			glm::vec3 dir = { edges[i + 3], edges[i + 4], edges[i + 5] };
			dir = glm::normalize(dir);

			this->edges[i + 3] = dir.x;
			this->edges[i + 4] = dir.y;
			this->edges[i + 5] = dir.z;
		}
	}
}

ConvexHull::~ConvexHull()
{
	delete[] planes;
	delete[] vertices;
	delete[] edges;
}