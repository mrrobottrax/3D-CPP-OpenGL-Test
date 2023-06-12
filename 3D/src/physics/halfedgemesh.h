#pragma once

#include <common/types.h>
#include <common/math.h>

struct heVertex;
struct heHalfEdge;
struct heEdge;
struct heFace;

struct heVertex
{
	glm::vec3 position;

	heVertex() : position(0, 0, 0) {};

	bool operator ==(heVertex& other)
	{
		return this->position == other.position;
	}
};

struct heHalfEdge
{
	heVertex* pTail;

	heHalfEdge* pPrev;
	heHalfEdge* pNext;
	heHalfEdge* pTwin;

	heFace* pFace;

	heHalfEdge() : pTail(), pPrev(), pNext(), pTwin(), pFace() {};

	bool operator ==(heHalfEdge& other)
	{
		return !memcmp(this, &other, sizeof(heHalfEdge));
	}
};

struct heEdge
{
	heHalfEdge* pHalfA;
	heHalfEdge* pHalfB;

	heEdge() : pHalfA(), pHalfB() {};
};

struct heFace
{
	heHalfEdge* pEdge;

	gmath::Plane plane;

	std::vector<glm::vec3> conflictList;

	heFace() : pEdge(), plane(), conflictList() {};

	bool operator ==(heFace& other)
	{
		return pEdge == other.pEdge && plane == other.plane;
	}
};

class HalfEdgeMesh
{
public:
	HalfEdgeMesh() : isConvex(), vertCount(), halfEdgeCount(), edgeCount(), faceCount(),
	verts(), halfEdges(), edges(), faces() {};
	~HalfEdgeMesh()
	{
		delete[] verts;
		delete[] halfEdges;
		delete[] edges;
		delete[] faces;
	};

	friend class QuickHull;

private:
	bool isConvex;

private:
	void CreateEdges();

public:
	inline bool IsConvex() { return isConvex; };

	void Draw(const float& time, const glm::vec3& position = { 0, 0, 0 },
		const glm::fquat& rotation = glm::identity<glm::fquat>(),
		const glm::vec3& scale = { 0, 0, 0 });

public:
	gSize_t vertCount;
	gSize_t halfEdgeCount;
	gSize_t edgeCount;
	gSize_t faceCount;

	heVertex* verts;
	heHalfEdge* halfEdges;
	heEdge* edges;
	heFace* faces;
};