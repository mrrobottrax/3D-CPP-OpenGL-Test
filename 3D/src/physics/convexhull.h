#pragma once

#include <common/math.h>

struct qhVertex;
struct qhHalfEdge;
struct qhFace;

struct qhVertex
{
	qhVertex* prev;
	qhVertex* next;

	qhHalfEdge* edge;

	float position[3];
};

struct qhHalfEdge
{
	qhVertex* tail;

	qhHalfEdge* prev;
	qhHalfEdge* next;
	qhHalfEdge* twin;

	qhFace* face;
};

struct qhFace
{
	qhFace* prev;
	qhFace* next;

	qhHalfEdge* edge;

	glm::vec3 normal;
	float dist;

	std::vector<qhVertex> conflictList;
};

class ConvexHull
{
public:
	ConvexHull() : vertices(), halfEdges(), faces(), halfEdgeCount(),
		epsilon(), sqrEpsilon()
	{
	};
	ConvexHull(const int vertCount, const glm::vec3* vertices);
	~ConvexHull();

public:
	float epsilon, sqrEpsilon;

private:
	void QuickHull(const int vertCount, const glm::vec3* vertices);
	void RemoveDuplicateVertices(std::list<glm::vec3>& vertices);
	void InitialHull(const std::list<glm::vec3>& vertices);

public:
	int halfEdgeCount;

	qhVertex* vertices;
	qhHalfEdge* halfEdges;
	qhFace* faces;
};