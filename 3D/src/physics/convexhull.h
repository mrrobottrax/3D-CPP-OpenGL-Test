#pragma once

#include <common/math.h>

struct qhVertex;
struct qhHalfEdge;
struct qhFace;

struct qhVertex
{
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
	qhHalfEdge* edge;

	glm::vec3 normal;
	float dist;

	std::vector<qhVertex> conflictList;
};

class ConvexHull
{
public:
	ConvexHull() : workingVertices(), epsilon(), sqrEpsilon() {};
	~ConvexHull() {};

	ConvexHull(const int vertCount, const glm::vec3* vertices);

public:
	float epsilon, sqrEpsilon;

private:
	void QuickHull(const int vertCount, const glm::vec3* verticesArray);
	void RemoveDuplicateVertices(std::list<glm::vec3>& vertices);
	void InitialHull(const std::list<glm::vec3>& vertices);
	void AddPoint(qhHalfEdge** horizon, int horizonSize, qhVertex& eye);

public:
	std::vector<qhVertex> workingVertices;
	std::list<qhHalfEdge> workingEdges;
	std::list<qhFace> workingFaces;
};