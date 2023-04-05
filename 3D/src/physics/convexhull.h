#pragma once

#include <common/math.h>

#include <unordered_set>

struct qhVertex;
struct qhHalfEdge;
struct qhFace;

struct qhVertex
{
	qhHalfEdge* edge;

	glm::vec3 position;

	qhVertex() : edge(), position() {};
};

struct qhHalfEdge
{
	qhVertex* tail;

	qhHalfEdge* prev;
	qhHalfEdge* next;
	qhHalfEdge* twin;

	qhFace* face;

	qhHalfEdge() : tail(), prev(), next(), twin(), face() {};
};

struct qhFace
{
	qhHalfEdge* edge;

	gMath::Plane plane;

	std::vector<glm::vec3> conflictList;

	qhFace() : edge(), plane(), conflictList() {};
};

class ConvexHull
{
public:
	ConvexHull() : workingVerts(), workingEdges(), workingFaces(), epsilon(), sqrEpsilon(),
	maxEdges(), maxVerts(), maxFaces(), nextEdgeIndex(), nextVertIndex(), nextFaceIndex() {};
	~ConvexHull();

	ConvexHull(const int vertCount, const glm::vec3* vertices);

public:
	float epsilon, sqrEpsilon;

private:
	void QuickHull(const int vertCount, const glm::vec3* verticesArray);
	void AllocateMemory(const int vertCount);
	void RemoveDuplicateVertices(std::list<glm::vec3>& vertices);
	void InitialHull(std::list<glm::vec3>& vertices);
	void AddPoint(qhHalfEdge** horizon, int horizonSize, glm::vec3& eye, std::vector<qhFace*>& newFaces, std::unordered_set<qhFace*>& oldFaces);

	qhHalfEdge* AddEdge();
	qhVertex* AddVertex();
	qhFace* AddFace();

	void RemoveEdge(qhHalfEdge& edge);
	void RemoveVertex(qhVertex& vertex);
	void RemoveFace(qhFace& face);

	void CondenseArrays();

	//TODO: make sure these functions are using correct precision
	void MergeCoplanar(const std::vector<qhHalfEdge*>& horizon);
	bool IsCoplanar(qhFace& a, qhFace& b);

public:
	int maxEdges;
	int maxVerts;
	int maxFaces;

	int nextEdgeIndex;
	int nextVertIndex;
	int nextFaceIndex;

	qhHalfEdge* workingEdges;
	qhVertex* workingVerts;
	qhFace* workingFaces;
};