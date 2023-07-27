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
};

class MalletMesh
{
public:
	mmHalfEdge* pFirstEdge = nullptr;

	void DeleteMesh();

	static void DeleteAllMeshes();
	static void UpdateRenderMesh(const Entity& entity);
	static void CalculateBounds(const Entity& entity);
};