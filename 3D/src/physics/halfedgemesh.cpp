#include "pch.h"
#include "halfedgemesh.h"

#include <systems/debugdraw.h>

void HalfEdgeMesh::CreateEdges()
{
	edgeCount = halfEdgeCount / 2;

	edges = new heEdge[edgeCount];

	for (int i = 0; i < edgeCount; ++i)
	{
		// Get next unused half edge
		for (int j = 0; j < halfEdgeCount; ++j)
		{
			heHalfEdge* pHalfEdge = &halfEdges[j];

			// Check if this half edge has already been added
			bool used = false;
			for (int k = 0; k < edgeCount; ++k)
			{
				const heEdge& edge = edges[k];

				if (edge.pHalfA == pHalfEdge || edge.pHalfB == pHalfEdge)
				{
					// Already added
					used = true;
					break;
				}
			}

			if (used)
			{
				continue;
			}

			// Set edge to point to this halfedge and its twin
			heEdge& edge = edges[i];

			edge.pHalfA = pHalfEdge;
			edge.pHalfB = pHalfEdge->pTwin;

			break;
		}
	}
}

void HalfEdgeMesh::Draw(const float& time, const glm::vec3& position, const glm::fquat& rotation, const glm::vec3& scale)
{
	for (int i = 0; i < edgeCount; ++i)
	{
		const heEdge& edge = edges[i];

		const glm::vec3& vertA = edge.pHalfA->pTail->position;
		const glm::vec3& vertB = edge.pHalfB->pTail->position;

		debugDraw.DrawLine(vertA, vertB, { 0, 1, 0 }, time);
	}
}
