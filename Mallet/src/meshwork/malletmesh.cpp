#include "malletpch.h"
#include "malletmesh.h"

#include <meshobject.h>
#include <components/meshcomponent.h>
#include <memory/entitymanager.h>
#include "malletmeshcomponent.h"

void DeleteMeshRecursive(mmHalfEdge* pEdge,
	std::unordered_set<glm::vec3*>& positions, std::unordered_set<mmVertex*>& vertices,
	std::unordered_set<mmHalfEdge*>& edges, std::unordered_set<mmFace*>& faces)
{
	if (!pEdge)
		return;

	if (edges.find(pEdge) != edges.end())
		return;

	edges.insert(pEdge);

	if (positions.find(pEdge->pTail->pPosition) == positions.end())
	{
		positions.insert(pEdge->pTail->pPosition);
	}

	if (vertices.find(pEdge->pTail) == vertices.end())
	{
		vertices.insert(pEdge->pTail);
	}

	if (faces.find(pEdge->pFace) == faces.end())
	{
		faces.insert(pEdge->pFace);
	}

	DeleteMeshRecursive(pEdge->pTwin, positions, vertices, edges, faces);
	DeleteMeshRecursive(pEdge->pNext, positions, vertices, edges, faces);
	DeleteMeshRecursive(pEdge->pPrev, positions, vertices, edges, faces);
}

void MalletMesh::DeleteMesh()
{
	std::unordered_set<glm::vec3*> positions;
	std::unordered_set<mmVertex*> vertices;
	std::unordered_set<mmHalfEdge*> edges;
	std::unordered_set<mmFace*> faces;

	DeleteMeshRecursive(pFirstEdge, positions, vertices, edges, faces);

	for (auto it = positions.begin(); it != positions.end(); ++it)
	{
		delete* it;
	}

	for (auto it = vertices.begin(); it != vertices.end(); ++it)
	{
		delete* it;
	}

	for (auto it = edges.begin(); it != edges.end(); ++it)
	{
		delete* it;
	}

	for (auto it = faces.begin(); it != faces.end(); ++it)
	{
		delete* it;
	}
}

void MalletMesh::DeleteAllMeshes()
{
	const std::vector<ChunkArchetypeElement*> archetypes = entityManager.FindChunkArchetypesWithComponent(Component().Init<MalletMeshComponent>());

	// For each archetype
	for (auto chunkArchetypeIt = archetypes.begin(); chunkArchetypeIt != archetypes.end(); ++chunkArchetypeIt)
	{
		// For each chunk
		for (Chunk* pChunk = (*chunkArchetypeIt)->pFirstChunk; pChunk != nullptr; pChunk = pChunk->pNext)
		{
			// For each entity
			for (gSize_t i = 0; i < pChunk->numberOfEntities; i++)
			{
				const EntityPointer p(pChunk, i);

				MalletMeshComponent& mMesh = entityManager.GetComponent<MalletMeshComponent>(p);
				MeshComponent& mesh = entityManager.GetComponent<MeshComponent>(p);

				mMesh.mesh.DeleteMesh();
				delete mesh.pMesh;
			}
		}
	}
}

void MalletMesh::UpdateRenderMesh(const Entity& entity)
{
	MeshComponent& meshComponent = entityManager.GetComponent<MeshComponent>(entity);

	if (!meshComponent.pMesh)
		meshComponent.pMesh = new MeshObject();

	MeshObject& mesh = *meshComponent.pMesh;
}

void MalletMesh::CalculateBounds(const Entity& entity)
{
}
