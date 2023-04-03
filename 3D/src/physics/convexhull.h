#pragma once

struct Vertex;
struct HalfEdge;
struct Face;

struct Vertex
{
	float position[3];
};

struct HalfEdge
{
	HalfEdge* twin;
	HalfEdge* next;
	Vertex* vertex;
	Face* face;
};

struct Face
{
	HalfEdge* halfEdge;

	glm::vec3 normal;
	float dist;
};

class ConvexHull
{
public:
	ConvexHull() : vertices(), halfEdges(), faces(), halfEdgeCount()
	{
	};
	ConvexHull(int vertCount, Vertex* vertices);
	~ConvexHull();

public:
	int halfEdgeCount;

	Vertex* vertices;
	HalfEdge* halfEdges;
	Face* faces;
};