#pragma once

#include <common/math.h>
#include <common/types.h>

#include <unordered_set>

struct qhVertex;
struct qhHalfEdge;
struct qhEdge;
struct qhFace;

struct qhVertex
{
	qhHalfEdge* pEdge;

	glm::vec3 position;

	qhVertex() : pEdge(), position(0, 0, 0) {};

	bool operator ==(qhVertex& other)
	{
		return this->pEdge == other.pEdge && this->position == other.position;
	}
};

struct qhHalfEdge
{
	qhVertex* pTail;

	qhHalfEdge* pPrev;
	qhHalfEdge* pNext;
	qhHalfEdge* pTwin;

	qhFace* pFace;

	qhHalfEdge() : pTail(), pPrev(), pNext(), pTwin(), pFace() {};

	bool operator ==(qhHalfEdge& other)
	{
		return !memcmp(this, &other, sizeof(qhHalfEdge));
	}
};

struct qhEdge
{
	qhHalfEdge* pHalfA;
	qhHalfEdge* pHalfB;

	qhEdge() : pHalfA(), pHalfB() {};
};

struct qhFace
{
	qhHalfEdge* pEdge;

	gmath::Plane plane;

	std::vector<glm::vec3> conflictList;

	qhFace() : pEdge(), plane(), conflictList() {};

	bool operator ==(qhFace& other)
	{
		return pEdge == other.pEdge && plane == other.plane;
	}
};

#ifdef DEBUG

//#define QHULL_DEBUG

#endif // DEBUG

#ifdef QHULL_DEBUG
const float delayTest = 0.1f;
#endif // QHULL_DEBUG

class ConvexHull
{
public:
	ConvexHull(const int vertCount, const glm::vec3* vertices);
	ConvexHull() : verts(), halfEdges(), edges(), faces(), epsilon(), sqrEpsilon(),
		vertCount(), halfEdgeCount(), edgeCount(), faceCount() {};
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
	void CreateEdges();

	//TODO: make sure these functions are using correct precision with fat planes etc
	void MergeCoplanar(const std::vector<qhHalfEdge*>& horizon, std::unordered_set<qhFace*>& unvisitedFaces);
	bool IsCoplanar(qhFace& a, qhFace& b);
	void RePartitionVertices(std::unordered_set<qhFace*> visibleFaces, std::vector<qhFace*> newFaces, const glm::vec3 eye);

public:
	gSize_t vertCount;
	gSize_t halfEdgeCount;
	gSize_t edgeCount;
	gSize_t faceCount;

	qhVertex* verts;
	qhHalfEdge* halfEdges;
	qhEdge* edges;
	qhFace* faces;
};