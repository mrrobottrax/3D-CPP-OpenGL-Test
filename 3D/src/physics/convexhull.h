#pragma once

struct Vertex;
struct HalfEdge;
struct Edge;
struct Face;
struct Plane;

struct Vertex
{
	float position[3];
};

struct HalfEdge
{
	HalfEdge* twin;
	HalfEdge* next;
	Vertex* vertex;
	Edge* edge;
	Face* face;
};

struct Edge
{
	HalfEdge* halfEdge;
};

struct Face
{
	HalfEdge* halfEdge;

	glm::vec3 normal;
	float dist;
};


struct Plane
{
	glm::vec3 normal;
	float dist;
};

class ConvexHull
{
public:
	ConvexHull() : vertices(), halfEdges(), edges(), faces(), halfEdgeCount()
	{
	};
	ConvexHull(int planeCount, Plane* planes);
	~ConvexHull();

public:
	int halfEdgeCount;

	Vertex* vertices;
	HalfEdge* halfEdges;
	Edge* edges;
	Face* faces;
};