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

	qhVertex() : edge(), position(0, 0, 0) {};

	bool operator ==(qhVertex& other)
	{
		return this->edge == other.edge && this->position == other.position;
	}
};

struct qhHalfEdge
{
	qhVertex* tail;

	qhHalfEdge* prev;
	qhHalfEdge* next;
	qhHalfEdge* twin;

	qhFace* face;

	qhHalfEdge() : tail(), prev(), next(), twin(), face() {};

	bool operator ==(qhHalfEdge& other)
	{
		return !memcmp(this, &other, sizeof(qhHalfEdge));
	}
};

struct qhFace
{
	qhHalfEdge* edge;

	gMath::Plane plane;

	std::vector<glm::vec3> conflictList;

	qhFace() : edge(), plane(), conflictList() {};

	bool operator ==(qhFace& other)
	{
		return edge == other.edge && plane == other.plane;
	}
};

#ifdef DEBUG

#define QHULL_DEBUG

#endif // DEBUG

#ifdef QHULL_DEBUG
const float delayTest = 1.0f;
#endif // QHULL_DEBUG

class ConvexHull
{
public:
	ConvexHull(const int vertCount, const glm::vec3* vertices);
	ConvexHull() : verts(), edges(), faces(), epsilon(), sqrEpsilon(),
		edgeCount(), vertCount(), faceCount() {};
	~ConvexHull();

public:
	float epsilon, sqrEpsilon;

public:
	void QuickHull(const int vertCount, const glm::vec3* verticesArray);

private:
	void AllocateMemory(const int vertCount);
	void RemoveDuplicateVertices(std::list<glm::vec3>& vertices);
	void InitialHull(std::list<glm::vec3>& vertices);
	void AddPoint(qhHalfEdge** horizon, int horizonSize, glm::vec3& eye, std::vector<qhFace*>& newFaces);

	qhHalfEdge* AddEdge();
	qhVertex* AddVertex();
	qhFace* AddFace();

	void RemoveEdge(qhHalfEdge& edge);
	void RemoveVertex(qhVertex& vertex);
	void RemoveFace(qhFace& face);

	void CondenseArrays(qhFace& startFace);

	//TODO: make sure these functions are using correct precision with fat planes etc
	void MergeCoplanar(const std::vector<qhHalfEdge*>& horizon, std::unordered_set<qhFace*>& unvisitedFaces);
	bool IsCoplanar(qhFace& a, qhFace& b);
	void RePartitionVertices(std::unordered_set<qhFace*> visibleFaces, std::vector<qhFace*> newFaces, const glm::vec3 eye);

public:
	size_t edgeCount;
	size_t vertCount;
	size_t faceCount;

	qhHalfEdge* edges;
	qhVertex* verts;
	qhFace* faces;
};