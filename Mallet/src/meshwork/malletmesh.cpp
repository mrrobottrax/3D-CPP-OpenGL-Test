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

void MalletMesh::CollectSets(std::unordered_set<glm::vec3*>& positions, std::unordered_set<mmVertex*>& vertices,
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

	CollectSets(positions, vertices, edges, faces);

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

void MalletMesh::UpdateRenderMesh(const EntityPointer& entity)
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

	CollectSets(positions, vertices, edges, faces);

	mesh.vertsSize = meshSize_t(vertices.size() * 3);
	mesh.normalsSize = meshSize_t(vertices.size() * 3);

	mesh.verts = new float[mesh.vertsSize];
	mesh.normals = new float[mesh.normalsSize];

	std::vector<meshIndex_t> indices;

	for (auto it = faces.begin(); it != faces.end(); ++it)
	{
		mmHalfEdge* const pFirstEdge = (*it)->pEdge;
		mmHalfEdge* pEdge = pFirstEdge;
		int vertCounter = 0;
		meshIndex_t startIndex;
		meshIndex_t prevIndex = 0;
		do {
			auto vertIt = vertices.find(pEdge->pTail);

			// Create in index so that verts can be reused
			const meshIndex_t index = static_cast<meshIndex_t>(std::distance(vertices.begin(), vertIt));
			const meshSize_t vertIndex = index * 3;

			// Keep track of the index of the first edge
			// for triangularization
			if (vertCounter == 0)
			{
				startIndex = index;
			}

			assert(vertIndex + 2 < mesh.normalsSize);
			assert(vertIndex + 2 < mesh.vertsSize);

			const glm::vec3& tail = *pEdge->pTail->pPosition;

			mesh.verts[vertIndex] = tail.x;
			mesh.verts[vertIndex + 1] = tail.y;
			mesh.verts[vertIndex + 2] = tail.z;

			const glm::vec3& normal = pEdge->pFace->normal;

			mesh.normals[vertIndex] = normal.x;
			mesh.normals[vertIndex + 1] = normal.y;
			mesh.normals[vertIndex + 2] = normal.z;

			// Triangle
			if (vertCounter >= 3)
			{
				indices.push_back(startIndex);
				indices.push_back(prevIndex);
			}

			indices.push_back(index);

			pEdge = pEdge->pNext;
			++vertCounter;
			prevIndex = index;
		} while (pEdge != pFirstEdge);
	}

	mesh.indicesCount = (meshSize_t)indices.size();
	mesh.indices = new meshIndex_t[mesh.indicesCount];
	std::copy(indices.begin(), indices.end(), mesh.indices);

	mesh.GenBuffers();

	//std::cout << "POSITIONS: " << positions.size() << std::endl;
	//for (auto it = positions.begin(); it != positions.end(); ++it)
	//{
	//	std::cout << (*it)->x << ", " << (*it)->y << ", " << (*it)->z << std::endl;
	//}

	//std::cout << "VERTS: " << mesh.vertsSize << std::endl;
	//for (int i = 0; i < mesh.vertsSize; i += 3)
	//{
	//	std::cout << mesh.verts[i] << ", ";
	//	std::cout << mesh.verts[i + 1] << ", ";
	//	std::cout << mesh.verts[i + 2] << "\n";
	//}

	//std::cout << "NORMALS: " << mesh.normalsSize << std::endl;
	//for (int i = 0; i < mesh.normalsSize; i += 3)
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
}

void MalletMesh::Validate(bool allowHoles)
{
	std::unordered_set<glm::vec3*> positions;
	std::unordered_set<mmVertex*> vertices;
	std::unordered_set<mmHalfEdge*> edges;
	std::unordered_set<mmFace*> faces;

	CollectSets(positions, vertices, edges, faces);

	for (auto it = positions.begin(); it != positions.end(); ++it)
	{
		const glm::vec3& vec = **it;
		assert(!glm::any(glm::isnan(vec)));
	}

	for (auto it = vertices.begin(); it != vertices.end(); ++it)
	{
		const mmVertex& vert = **it;
		assert(!glm::any(glm::isnan(vert.uv)));
		assert(vert.pPosition);
	}

	for (auto it = edges.begin(); it != edges.end(); ++it)
	{
		const mmHalfEdge& edge = **it;
		assert(edge.pFace);
		assert(edge.pNext);
		assert(edge.pNext->pPrev == &edge);
		assert(edge.pPrev);
		assert(edge.pPrev->pNext == &edge);
		assert(edge.pTail);

		if (!allowHoles || edge.pTwin)
		{
			assert(edge.pTwin);
			assert(edge.pTwin->pTwin == &edge);
		}
	}

	for (auto it = faces.begin(); it != faces.end(); ++it)
	{
		const mmFace& face = **it;
		assert(!glm::any(glm::isnan(face.normal)));
		assert(face.pEdge);
		assert(face.pEdge->pFace == &face);
	}
}
