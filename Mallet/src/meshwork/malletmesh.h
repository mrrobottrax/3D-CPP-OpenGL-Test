#pragma once
#include <glm.hpp>
#include <memory/entity.h>

struct mmVertex;
struct mmHalfEdge;
struct mmFace;

struct mmVertex
{
	glm::vec3* pPosition;
	glm::vec2 uv;
};

struct mmHalfEdge
{
	mmVertex* pTail = nullptr;

	mmHalfEdge* pPrev = nullptr;
	mmHalfEdge* pTwin = nullptr;
	mmHalfEdge* pNext = nullptr;

	mmFace* pFace = nullptr;
};

struct mmFace
{
	mmHalfEdge* pEdge = nullptr;
	glm::vec3 normal = glm::vec3(0);
};

class MalletMesh
{
public:
	mmHalfEdge* pFirstEdge = nullptr;

	void CollectSets(std::unordered_set<glm::vec3*>& positions, std::unordered_set<mmVertex*>& vertices,
		std::unordered_set<mmHalfEdge*>& edges, std::unordered_set<mmFace*>& faces);
	void DeleteMesh();
	void UpdateRenderMesh(const Entity& entity);
	void Validate(bool allowHoles = false);

	static void DeleteAllMeshes();
	static void CalculateBounds(const Entity& entity);
};