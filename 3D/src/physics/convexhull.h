#pragma once

struct Plane
{
	glm::vec3 normal;
	float dist;
};

class ConvexHull
{
public:
	ConvexHull() : planeCount(), planes(), vertCount(), vertices(), edgeCount(), edges()
	{
	};

	ConvexHull(unsigned short planeCount, Plane* planes, unsigned short vertCount, float* vertices, unsigned short edgeCount, float* edges);
	~ConvexHull();

public:
	unsigned short planeCount;
	unsigned short vertCount;
	unsigned short edgeCount;

	Plane* planes;
	float* vertices;
	float* edges; // Edges are stored as a head and a direction
};