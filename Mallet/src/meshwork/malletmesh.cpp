#include "malletpch.h"
#include "malletmesh.h"

#include <meshobject.h>
#include <components/meshcomponent.h>
#include <memory/entitymanager.h>
#include "malletmeshcomponent.h"
#include <components/positioncomponent.h>
#include <components/rotationcomponent.h>
#include <components/scalecomponent.h>

void CollectVectorsRecursive(mmHalfEdge* pEdge,
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

	CollectVectorsRecursive(pEdge->pTwin, positions, vertices, edges, faces);
	CollectVectorsRecursive(pEdge->pNext, positions, vertices, edges, faces);
	CollectVectorsRecursive(pEdge->pPrev, positions, vertices, edges, faces);
}

void MalletMesh::CollectVectors(std::unordered_set<glm::vec3*>& positions, std::unordered_set<mmVertex*>& vertices,
	std::unordered_set<mmHalfEdge*>& edges, std::unordered_set<mmFace*>& faces)
{
	CollectVectorsRecursive(pFirstEdge, positions, vertices, edges, faces);
}

void MalletMesh::DeleteMesh()
{
	std::unordered_set<glm::vec3*> positions;
	std::unordered_set<mmVertex*> vertices;
	std::unordered_set<mmHalfEdge*> edges;
	std::unordered_set<mmFace*> faces;

	CollectVectors(positions, vertices, edges, faces);

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

	if (meshComponent.pMesh)
		delete meshComponent.pMesh;

	meshComponent.pMesh = new MeshObject();
	MeshObject& mesh = *meshComponent.pMesh;

	std::unordered_set<glm::vec3*> positions;
	std::unordered_set<mmVertex*> vertices;
	std::unordered_set<mmHalfEdge*> edges;
	std::unordered_set<mmFace*> faces;

	CollectVectors(positions, vertices, edges, faces);

	mesh.vertCount = meshSize_t(vertices.size() * 3);
	mesh.normalCount = meshSize_t(vertices.size() * 3);
	mesh.indicesCount = meshSize_t(vertices.size());

	mesh.verts = new float[mesh.vertCount];
	mesh.normals = new float[mesh.normalCount];
	mesh.indices = new meshIndex_t[mesh.indicesCount]{};

	int i = 0;
	for (auto it = faces.begin(); it != faces.end(); ++it)
	{
		mmHalfEdge* const pFirstEdge = (*it)->pEdge;
		mmHalfEdge* pEdge = pFirstEdge;
		do {
			assert(i < mesh.indicesCount);

			auto vertIt = vertices.find(pEdge->pTail);

			// Create in index so that verts can be reused
			const int index = (int)std::distance(vertices.begin(), vertIt);
			const int vertIndex = index * 3;
			assert(vertIndex + 2 < mesh.normalCount);
			assert(vertIndex + 2 < mesh.vertCount);

			const glm::vec3& tail = *pEdge->pTail->pPosition;

			mesh.verts[vertIndex] = tail.x;
			mesh.verts[vertIndex + 1] = tail.y;
			mesh.verts[vertIndex + 2] = tail.z;

			const glm::vec3& normal = pEdge->pFace->normal;

			mesh.normals[vertIndex] = normal.x;
			mesh.normals[vertIndex + 1] = normal.y;
			mesh.normals[vertIndex + 2] = normal.z;

			mesh.indices[i] = index;

			pEdge = pEdge->pNext;
			++i;
		} while (pEdge != pFirstEdge);
	}

	//std::cout << "VERTS: " << mesh.vertCount << std::endl;
	//for (int i = 0; i < mesh.vertCount; i += 3)
	//{
	//	std::cout << mesh.verts[i] << ", ";
	//	std::cout << mesh.verts[i + 1] << ", ";
	//	std::cout << mesh.verts[i + 2] << "\n";
	//}

	//std::cout << "NORMALS: " << mesh.normalCount << std::endl;
	//for (int i = 0; i < mesh.normalCount; i += 3)
	//{
	//	std::cout << mesh.normals[i] << ", ";
	//	std::cout << mesh.normals[i + 1] << ", ";
	//	std::cout << mesh.normals[i + 2] << "\n";
	//}

	//std::cout << "INDICES: " << mesh.indicesCount << std::endl;
	//for (int i = 0; i < mesh.indicesCount; ++i)
	//{
	//	std::cout << mesh.indices[i] << "\n";
	//}

	mesh.GenBuffers();
}

void MalletMesh::CalculateBounds(const Entity& entity)
{
}
