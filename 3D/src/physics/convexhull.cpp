#include <pch.h>
#include <physics/convexhull.h>

#include <debugtools/debugdraw.h>
#include <unordered_set>

#include <chrono>
#include <thread>

using namespace gMath;

// Debug functions
void DrawPoint(glm::vec3 point, glm::vec3 color = { 1, 1, 1 }, float time = FLT_MAX)
{
	debugDraw.DrawLine(point, point + glm::vec3(0, 0.1f, 0), color, time);
}

void DrawPolygonEdges(qhHalfEdge& startEdge, const glm::vec3& color = { 1, 1, 1 }, const float time = FLT_MAX, const float explode = 0)
{
	int i = 0;
	qhHalfEdge* pEdge = &startEdge;
	glm::vec3 add(0);
	// Explode faces a little
	if (startEdge.pFace)
	{
		add = startEdge.pFace->plane.normal * explode;
	}
	do {
		glm::vec3 start = { (*pEdge).pTail->position[0], (*pEdge).pTail->position[1], (*pEdge).pTail->position[2] };
		glm::vec3 end = { (*pEdge).pNext->pTail->position[0], (*pEdge).pNext->pTail->position[1], (*pEdge).pNext->pTail->position[2] };

		debugDraw.DrawLine(start + add, end + add, color, time);

		pEdge = pEdge->pNext;
		++i;
	} while (pEdge != &startEdge);
}

void DrawHull(const qhFace& startFace, const float time = FLT_MAX, const bool points = false, const float normalDist = 0)
{
	std::unordered_set<qhFace*> visited;

	// Depth first search
	std::function<void(const qhFace&)> DrawHullRecursive = [&](const qhFace& face)
	{
		qhHalfEdge& startEdge = *face.pEdge;
		qhHalfEdge* pEdge = &startEdge;
		do {
			// Only recurse when the edge is not already in the list
			if (visited.find(pEdge->pTwin->pFace) == visited.end())
			{
				visited.insert(pEdge->pTwin->pFace);

				if (pEdge->pTwin != nullptr && pEdge->pTwin->pFace != nullptr)
				{
					DrawHullRecursive(*pEdge->pTwin->pFace);
				}
				else
				{
					printf("QHull Error: Drawing bad face");
					DrawPolygonEdges(*pEdge, { 1, 1, 0 }, 60, 0.05f);
					debugDraw.DrawLine(pEdge->pTail->position, pEdge->pNext->pTail->position, { 0, 0, 1 }, 60);
					std::this_thread::sleep_for(std::chrono::seconds(120));
				}

			}

			pEdge = pEdge->pNext;
		} while (pEdge != &startEdge);
	};

	DrawHullRecursive(startFace);

	// Draw each face
	for (auto it = visited.begin(); it != visited.end(); ++it)
	{
		glm::vec3 color{0, 1, 1};
			
		glm::vec3 center(0);
		int divide = 0;

		glm::vec3 add = (*it)->plane.normal * 0.02f;

		qhHalfEdge& startEdge = *(*it)->pEdge;
		qhHalfEdge* pEdge = &startEdge;
		do {
			debugDraw.DrawLine(pEdge->pTail->position + add, pEdge->pNext->pTail->position + add, color, time);

			center += pEdge->pTail->position;
			++divide;

			pEdge = pEdge->pNext;
		} while (pEdge != &startEdge);

		center /= divide;

		if (normalDist != 0)
			debugDraw.DrawLine(center, center + (*it)->plane.normal * normalDist, color, time);

		if (points)
		{
			for (auto p : (*it)->conflictList)
			{
				DrawPoint(p, color, time);
			}
		}
	}
}

float CalcEpsilon(const std::list<glm::vec3>& verticesList)
{
	// Calculate epsilon based on bounds
	float epsilon;
	float max[3] = { 0, 0, 0 };

	for (auto it = verticesList.begin(); it != verticesList.end(); ++it)
	{
		for (int i = 0; i < 3; ++i)
		{
			max[i] = std::max(max[i], std::abs((*it)[i]));
		}
	}

	epsilon = 3 * (max[0] + max[1] + max[2]) * FLT_EPSILON;

	return epsilon;
}

void ConvexHull::RemoveDuplicateVertices(std::list<glm::vec3>& vertices)
{
	for (auto it = vertices.begin(); it != vertices.end(); ++it)
	{
		glm::vec3& vertex = *it;

		auto it2 = it;
		++it2;
		while (it2 != vertices.end())
		{
			glm::vec3& vertex2 = *it2;

			// Check if distance is less than epsilon
			if (SqrDist(vertex, vertex2) <= sqrEpsilon)
			{
				std::cout << "QHULL WARNING: DUPLICATE VERTICES DETECTED AND REMOVED: "
					<< vertex[0] << ", " << vertex[1] << ", " << vertex[2] << " : "
					<< vertex2[0] << ", " << vertex2[1] << ", " << vertex2[2] << "\n";

				vertices.erase(it2++);
			}
			else
			{
				++it2;
			}
		}
	}
}

void ConnectEdgeLoop(qhHalfEdge** edges, int edgeCount)
{
	// Set next and prev edges
	for (int j = 0; j < edgeCount; ++j)
	{
		if (j == 0)
			edges[j]->pPrev = edges[edgeCount - 1];
		else
			edges[j]->pPrev = edges[(j - 1) % edgeCount];

		edges[j]->pNext = edges[(j + 1) % edgeCount];
	}
}

void ConvexHull::RePartitionVertices(std::unordered_set<qhFace*> visibleFaces, std::vector<qhFace*> newFaces, const glm::vec3 eye)
{
	for (auto it = visibleFaces.begin(); it != visibleFaces.end(); ++it)
	{
		const qhFace& face = **it;

		// Parition vertices
		for (int p = 0; p < face.conflictList.size(); ++p)
		{
			const glm::vec3& point = face.conflictList[p];

			// The eye is no longer a conflict point
			if (point == eye)
			{
				continue;
			}

			// Find closest plane
			float bestDist = FLT_MAX;
			qhFace* pBestFace = nullptr;
			for (int i = 0; i < newFaces.size(); ++i)
			{
				qhFace& conflictFace = *newFaces[i];
				float dist = SignedDistFromPlane(conflictFace.plane, point);
				if (dist >= 0)
					dist -= epsilon;

				if (dist <= bestDist && dist > 0)
				{
					pBestFace = &conflictFace;
				}
			}

			// Point is outside
			if (pBestFace)
			{
				pBestFace->conflictList.push_back(point);
			}
		}
	}
}

bool ConvexHull::IsCoplanar(qhFace& a, qhFace& b)
{
	// Get center of face by averaging vertices
	glm::vec3 bCenter = glm::vec3(0);
	{
		int divide = 0;

		qhHalfEdge& startEdge = *b.pEdge;
		qhHalfEdge* pEdge = &startEdge;
		do {
			bCenter += pEdge->pTail->position;

			pEdge = pEdge->pNext;
			++divide;
		} while (pEdge != &startEdge);

		bCenter /= divide;
	}

	// Check if center is above A (using fat planes)
	float dist = std::abs(glm::dot(bCenter, a.plane.normal) - a.plane.dist);
	if (dist <= epsilon)
	{
		return true;
	}

	return false;
}

void ConvexHull::MergeCoplanar(const std::vector<qhHalfEdge*>& horizon, std::unordered_set<qhFace*>& unvisitedFaces)
{
	for (int i = 0; i < horizon.size(); ++i)
	{
		qhHalfEdge* pEdge = horizon[i];
		qhFace& face1 = *pEdge->pTwin->pFace;
		qhFace& face2 = *pEdge->pFace;

		if (!IsCoplanar(face1, face2))
			continue;

		// ~~~~~~~~~~~~~~~~~Merge faces ~~~~~~~~~~~~~~~~~

		// Absorb conflict points
		// face 1 absorbs face 2 so that continuous face merging works
		for (int i = 0; i < face2.conflictList.size(); ++i)
			face1.conflictList.push_back(face2.conflictList[i]);

		// Set face references in edges
		{
			qhHalfEdge& startEdge2 = *face2.pEdge;
			qhHalfEdge* pEdge2 = &startEdge2;
			do {
				pEdge2->pFace = &face1;

				pEdge2 = pEdge2->pNext;
			} while (pEdge2 != &startEdge2);
		}

		qhHalfEdge& seperatingEdgeANext = *pEdge->pTwin->pNext;
		qhHalfEdge& seperatingEdgeAPrev = *pEdge->pTwin->pPrev;

		qhHalfEdge& seperatingEdgeBNext = *pEdge->pNext;
		qhHalfEdge& seperatingEdgeBPrev = *pEdge->pPrev;

		// Set edge reference in face
		face1.pEdge = &seperatingEdgeANext;

		// Connect edges
		seperatingEdgeAPrev.pNext = &seperatingEdgeBNext;
		seperatingEdgeANext.pPrev = &seperatingEdgeBPrev;

		seperatingEdgeBPrev.pNext = &seperatingEdgeANext;
		seperatingEdgeBNext.pPrev = &seperatingEdgeAPrev;

		// Remove old stuff
		unvisitedFaces.erase(&face2);
		RemoveFace(face2);
		RemoveEdge(*pEdge->pTwin);
		RemoveEdge(*pEdge);

		// TODO: RECALCULATE FACE W/ NEWELL PLANE
	}
}

void ConvexHull::InitialHull(std::list<glm::vec3>& points)
{
	// Create initial hull with min and max verts in each axis
	glm::vec3 bounds[6] = { glm::vec3(FLT_MAX), glm::vec3(FLT_MAX), glm::vec3(FLT_MAX), glm::vec3(FLT_MIN), glm::vec3(FLT_MIN), glm::vec3(FLT_MIN) };
	{
		glm::vec3* mins = &bounds[0];
		glm::vec3* maxs = &bounds[3];

		for (auto it = points.begin(); it != points.end(); ++it)
		{
			// Mins
			for (int i = 0; i < 3; ++i)
			{
				const glm::vec3& vert = *it;

				if (vert[i] <= mins[i][i])
				{
					mins[i] = vert;
				}
			}

			// Maxs
			for (int i = 0; i < 3; ++i)
			{
				const glm::vec3& vert = *it;

				if (vert[i] >= maxs[i][i])
				{
					maxs[i] = vert;
				}
			}
		}
	}

	// Find two furthest vertices
	glm::vec3 initialHullPoints[4] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
	{
		float bestSqrDist = 0;
		for (int i = 0; i < 6; ++i)
		{
			glm::vec3& vertA = bounds[i];
			for (int j = 0; j < 6; ++j)
			{
				glm::vec3& vertB = bounds[j];

				float dist = SqrDist(vertA, vertB);
				if (dist >= bestSqrDist)
				{
					bestSqrDist = dist;
					initialHullPoints[0] = vertA;
					initialHullPoints[1] = vertB;
				}
			}
		}
		points.remove(initialHullPoints[0]);
		points.remove(initialHullPoints[1]);
	}

	// Get furthest point from line
	{
		float bestDist = 0;
		for (auto it = points.begin(); it != points.end(); ++it)
		{
			float dist = DistFromLine(initialHullPoints[0], initialHullPoints[1], *it);
			if (dist >= bestDist)
			{
				initialHullPoints[2] = *it;
				bestDist = dist;
			}
		}
		points.remove(initialHullPoints[2]);
	}

	// Get furthest point from plane
	Plane basePlane = PlaneFromTri(initialHullPoints[0], initialHullPoints[1], initialHullPoints[2]);
	bool planeFlipped = false;
	{
		float bestDist = 0;
		for (auto it = points.begin(); it != points.end(); ++it)
		{
			float dist = std::abs(SignedDistFromPlane(basePlane, *it));
			if (dist >= bestDist)
			{
				initialHullPoints[3] = *it;
				bestDist = dist;
			}
		}
		points.remove(initialHullPoints[3]);

		// Flip plane if needed
		if (glm::dot(basePlane.normal, initialHullPoints[3]) - basePlane.dist >= 0)
		{
			basePlane.Invert();
			planeFlipped = true;
		}
	}

	// Add base vertices to hull
	qhVertex* baseVerts[3];
	for (int i = 0; i < 3; ++i)
	{
		qhVertex& vertex = *AddVertex();
		vertex.position = { initialHullPoints[i][0], initialHullPoints[i][1], initialHullPoints[i][2] };
		baseVerts[i] = &vertex;
	}

	// Construct initial hull
	// Create base
	qhHalfEdge* baseEdges[3];
	if (!planeFlipped)
	{
		baseVerts[0]->pEdge = baseEdges[0] = AddEdge();
		baseEdges[0]->pTail = baseVerts[0];
		
#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[0]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		baseVerts[1]->pEdge = baseEdges[1] = AddEdge();
		baseEdges[1]->pTail = baseVerts[1];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[1]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		baseVerts[2]->pEdge = baseEdges[2] = AddEdge();
		baseEdges[2]->pTail = baseVerts[2];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[2]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

	}
	else // Swap order of vertices to make face counter clockwise
	{
		baseVerts[2]->pEdge = baseEdges[0] = AddEdge();
		baseEdges[0]->pTail = baseVerts[2];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[0]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		baseVerts[1]->pEdge = baseEdges[1] = AddEdge();
		baseEdges[1]->pTail = baseVerts[1];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[1]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		baseVerts[0]->pEdge = baseEdges[2] = AddEdge();
		baseEdges[2]->pTail = baseVerts[0];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[2]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

	}

	ConnectEdgeLoop(baseEdges, 3);

	qhFace* tetrahedronFaces[4];

	// Connect edges to face
	{
		tetrahedronFaces[0] = AddFace();
		tetrahedronFaces[0]->pEdge = baseEdges[0];
		tetrahedronFaces[0]->plane = {
				basePlane.normal,
				basePlane.dist
		};

		for (int i = 0; i < 3; ++i)
		{
			baseEdges[i]->pFace = tetrahedronFaces[0];
		}
	}

	// Reverse edge order for horizon to be counter clockwise
	{
		qhHalfEdge* pTempEdge = baseEdges[0];
		baseEdges[0] = baseEdges[1];
		baseEdges[1] = pTempEdge;
	}

	// Add fourth vertex
	{
		std::vector<qhFace*> newFaces;
		AddPoint(baseEdges, 3, initialHullPoints[3], newFaces);

		// Add new faces to array
		for (int i = 0; i < 3; ++i)
		{
			tetrahedronFaces[i + 1] = newFaces[i];
		}
	}

	// Partition points
	for (auto it = points.begin(); it != points.end(); ++it)
	{
		const glm::vec3& vertex = (*it);

		// Find closest plane
		float bestDist = FLT_MAX;
		qhFace* pBestFace = nullptr;
		for (auto f : tetrahedronFaces)
		{
			float dist = SignedDistFromPlane(f->plane, vertex);
			if (dist >= 0)
				dist -= epsilon;

			if (dist <= bestDist && dist > 0)
			{
				pBestFace = f;
			}
		}

		// Vertex is outside
		if (pBestFace)
		{
			pBestFace->conflictList.push_back(vertex);
		}
	}
}

void ConvexHull::AddPoint(qhHalfEdge** horizon, const int horizonSize, glm::vec3& eye, std::vector<qhFace*>& newFaces)
{
	// Create vertex
	qhVertex& eyeVert = *AddVertex();
	eyeVert.position = eye;

	qhHalfEdge* pLoopEdge = nullptr; // The edge that the loop eventually has to connect back to
	qhHalfEdge* pConnectingEdge = nullptr;
	for (int i = 0; i < horizonSize; ++i)
	{
		qhHalfEdge& edge = *horizon[i];
		qhHalfEdge& nextEdge = *edge.pNext;

		qhHalfEdge* edges[3]{};

		// Create edges
		edges[0] = AddEdge();
		edges[0]->pTail = nextEdge.pTail;

		edges[1] = AddEdge();
		edges[1]->pTail = edge.pTail;

		edges[2] = AddEdge();
		edges[2]->pTail = &eyeVert;

		Plane plane = PlaneFromTri(edges[0]->pTail->position, edges[1]->pTail->position, edges[2]->pTail->position);

		if (pConnectingEdge != nullptr)
		{
			edges[2]->pTwin = pConnectingEdge;
			pConnectingEdge->pTwin = edges[2];
		}
		else // We are on the first face
		{
			pLoopEdge = edges[2];
		}

		ConnectEdgeLoop(edges, 3);

		edges[0]->pTwin = &edge;
		edge.pTwin = edges[0];

		// Create face
		newFaces.push_back(AddFace());
		newFaces.back()->pEdge = edges[0];
		newFaces.back()->plane = {
			plane.normal,
			plane.dist
		};

		// Add face reference to edges
		for (int i = 0; i < 3; ++i)
		{
			edges[i]->pFace = newFaces.back();
		}

		pConnectingEdge = edges[1];
	}

// False positive
// C6011: Dereferencing NULL pointer
#pragma warning( push )
#pragma warning( disable : 6011 )
	pConnectingEdge->pTwin = pLoopEdge;
	pLoopEdge->pTwin = pConnectingEdge;
#pragma warning( pop )
}

bool FaceIsVisible(const qhFace& face, const glm::vec3 eye)
{
	return gMath::SignedDistFromPlane(face.plane, eye) > 0;
}

void ConvexHull::QuickHull(const int vertCount, const glm::vec3* verticesArray)
{
	//Setup
	{
		std::list<glm::vec3> verticesList;
		std::copy(verticesArray, &verticesArray[vertCount], std::back_inserter(verticesList));

		epsilon = CalcEpsilon(verticesList);
		sqrEpsilon = epsilon * epsilon;

		RemoveDuplicateVertices(verticesList);

		AllocateMemory((int)verticesList.size());

		InitialHull(verticesList);
	}

#ifdef QHULL_DEBUG
	DrawHull(faces[0], 5, true, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * 5)));
#endif // QHULL_DEBUG

	// For each face
	qhFace* pLastFace = nullptr;
	std::unordered_set<qhFace*> unvisitedFaces;
	for (int i = 0; i < 4; ++i)
		unvisitedFaces.insert(&faces[i]);

	while (unvisitedFaces.size() > 0)
	{
		qhFace& face = **unvisitedFaces.begin();

		unvisitedFaces.erase(&face);

		if (&face == nullptr)
		{
			std::cout << "QHULL ERROR: Face is null\n";
			continue;
		}

#ifdef QHULL_DEBUG
		DrawPolygonEdges(*face.edge, { 0.5f, 1, 0 }, delayTest, 0.25f);
		DrawHull(face, delayTest, true, 0);
		for (auto p : face.conflictList)
		{
			DrawPoint(p, { 0.5, 1, 0 }, delayTest);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		if (face.conflictList.size() <= 0)
		{
			continue;
		}

		// Find furthest conflict point
		glm::vec3* pEye = nullptr;
		{
			float bestDist = 0;
			for (int p = 0; p < face.conflictList.size(); ++p)
			{
				glm::vec3& point = face.conflictList[p];

				float dist = gMath::SignedDistFromPlane(face.plane, point);
				if (dist >= bestDist)
				{
					bestDist = dist;
					pEye = &point;
				}
			}

			if (pEye == nullptr)
			{
				std::cout << "QHULL ERROR: COULDN'T FIND EYE POINT\n";
				continue;
			}
		}

		// Calculate horizon
		std::vector<qhHalfEdge*> horizon;
		std::unordered_set<qhFace*> visible;
		{
			// Find visible faces
			std::function<void(qhHalfEdge&, const glm::vec3)> FindVisibleRecursive =
				[&FindVisibleRecursive, &visible, &horizon](qhHalfEdge& startEdge, const glm::vec3 eye) -> void
			{
				qhFace& face = *startEdge.pFace;

				visible.insert(&face);

				// Recurse on face edges
				{
					qhHalfEdge* pEdge = &startEdge;
					do
					{
						qhFace& twinFace = *pEdge->pTwin->pFace;

						bool twinVisible = FaceIsVisible(twinFace, eye);

						if (visible.find(&twinFace) == visible.end() && twinVisible)
						{
							// Face has not been visited
							FindVisibleRecursive(*pEdge->pTwin, eye);
						}

						if (!twinVisible)
						{
							horizon.push_back(pEdge->pTwin);
						}

						pEdge = pEdge->pNext;
					} while (pEdge != &startEdge);
				}
			};

			FindVisibleRecursive(*face.pEdge, *pEye);

#ifdef QHULL_DEBUG
			for (auto f : visible)
			{
				DrawPolygonEdges(*f->edge, { 1, 0, 1 }, delayTest, 0.25f);
			}

			DrawHull(face, delayTest, true, 0);

			DrawPoint(*eye, { 1, 1, 1 }, delayTest);

			std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		}

		if (horizon.size() <= 0)
		{
			continue;
		}

		// Connect eye to horizon
		std::vector<qhFace*> newFaces;
		AddPoint(&horizon[0], (int)horizon.size(), *pEye, newFaces);

		for (auto f : newFaces)
		{
			unvisitedFaces.insert(f);
		}

#ifdef QHULL_DEBUG
		DrawHull(**newFaces.begin(), delayTest, true, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		// Repartition all old face conflict vertices
		RePartitionVertices(visible, newFaces, *pEye);

		// Merge coplanar faces
		MergeCoplanar(horizon, unvisitedFaces);

#ifdef QHULL_DEBUG
		DrawHull(**newFaces.begin(), delayTest, true, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		// Remove obscured faces
		for (auto it = visible.begin(); it != visible.end(); ++it)
		{
			qhHalfEdge& startEdge = *(*it)->pEdge;
			qhHalfEdge* pEdge = &startEdge;
			do {
				qhHalfEdge* pEdge2 = pEdge;

				pEdge = pEdge->pNext;

				RemoveEdge(*pEdge2);
			} while (pEdge != &startEdge);

			unvisitedFaces.erase(*it);
			RemoveFace(**it);
		}

		pLastFace = newFaces[0];
	}

	if (pLastFace != nullptr)
		CondenseArrays(*pLastFace);

#ifdef QHULL_DEBUG
	if (faceCount > 0)
		DrawHull(faces[0], 120, false, 0.5f);
#endif // QHULL_DEBUG

}

qhHalfEdge* ConvexHull::AddEdge()
{
	for (int i = 0; i < edgeCount; ++i)
	{
		if (edges[i] == qhHalfEdge())
		{
			return &edges[i];
		}
	}

	return nullptr;
}

qhVertex* ConvexHull::AddVertex()
{
	for (int i = 0; i < vertCount; ++i)
	{
		if (verts[i] == qhVertex())
		{
			return &verts[i];
		}
	}

	return nullptr;
}

qhFace* ConvexHull::AddFace()
{
	for (int i = 0; i < faceCount; ++i)
	{
		if (faces[i] == qhFace())
		{
			return &faces[i];
		}
	}

	return nullptr;
}

void ConvexHull::RemoveEdge(qhHalfEdge& edge)
{
	edge = qhHalfEdge();
}

void ConvexHull::RemoveVertex(qhVertex& vertex)
{
	// Multiple edges reference vertices
	// Also I'm pretty sure there's no need for QHull to remove a vertex
}

void ConvexHull::RemoveFace(qhFace& face)
{
	face = qhFace();
}

void ConvexHull::AllocateMemory(const int pointCount)
{
	this->vertCount = pointCount;
	this->edgeCount = 3 * pointCount - 6;
	this->faceCount = 2 * pointCount - 4;

	// Extra room for extra stuff creating during QHull
	this->vertCount *= 2;
	this->edgeCount *= 2;
	this->faceCount *= 2;

	this->edges = new qhHalfEdge[this->edgeCount];
	for (int i = 0; i < edgeCount; ++i)
	{
		this->edges[i] = qhHalfEdge();
	}

	this->verts = new qhVertex[this->vertCount];
	for (int i = 0; i < vertCount; ++i)
	{
		this->verts[i] = qhVertex();
	}

	this->faces = new qhFace[this->faceCount];
	for (int i = 0; i < faceCount; ++i)
	{
		this->faces[i] = qhFace();
	}
}

void ConvexHull::CondenseArrays(qhFace& startFace)
{
	std::unordered_set<qhFace*> visitedFaces;
	std::unordered_set<qhHalfEdge*> visitedEdges;
	std::unordered_set<qhVertex*> visitedVertices;

	// Get hull as arrays
	std::function<void(qhFace&)> GetDataRecursive = [&](qhFace& startFace)
	{
		qhHalfEdge& startEdge = *startFace.pEdge;
		qhHalfEdge* pEdge = &startEdge;
		do {
			visitedEdges.insert(pEdge);
			visitedVertices.insert(pEdge->pTail);

			// Only recurse when the face is not already in the list
			if (visitedFaces.find(pEdge->pTwin->pFace) == visitedFaces.end())
			{
				visitedFaces.insert(pEdge->pTwin->pFace);
				GetDataRecursive(*(pEdge->pTwin->pFace));
			}

			pEdge = pEdge->pNext;
		} while (pEdge != &startEdge);
	};

	GetDataRecursive(startFace);

	vertCount = visitedVertices.size();
	edgeCount = visitedEdges.size();
	faceCount = visitedFaces.size();

	qhVertex* vertArray = new qhVertex[vertCount];
	qhHalfEdge* edgeArray = new qhHalfEdge[edgeCount];
	qhFace* faceArray = new qhFace[faceCount];

	// Move all verts
	{
		auto UpdatePointer = [](qhVertex*& pointer, qhVertex* pTest, qhVertex* pSet)
		{
			if (pointer == pTest)
			{
				pointer = pSet;
			}
		};

		int i = 0;

		for (auto it = visitedVertices.begin(); it != visitedVertices.end(); ++it)
		{
			qhVertex* pVert = *it;

			assert(i < vertCount);
			vertArray[i] = *pVert;

			qhVertex* pNew = &vertArray[i];

			// Update pointers to this vertex
			for (auto edgeIt = visitedEdges.begin(); edgeIt != visitedEdges.end(); ++edgeIt)
			{
				qhHalfEdge& edge = **edgeIt;

				UpdatePointer(edge.pTail, pVert, pNew);
			}

			++i;
		}
	}

	// Move all edges
	{
		auto UpdatePointer = [](qhHalfEdge*& pointer, qhHalfEdge* pTest, qhHalfEdge* pSet)
		{
			if (pointer == pTest)
			{
				pointer = pSet;
			}
		};

		int i = 0;

		for (auto it = visitedEdges.begin(); it != visitedEdges.end(); ++it)
		{
			qhHalfEdge* pEdge = *it;

			assert(i < edgeCount);
			edgeArray[i] = *pEdge;

			qhHalfEdge* pNew = &edgeArray[i];

			// Update pointers to this edge
			for (int vertIndex = 0; vertIndex < vertCount; ++vertIndex)
			{
				qhVertex& vert = vertArray[vertIndex];

				UpdatePointer(vert.pEdge, pEdge, pNew);
			}

			for (auto edgeIt = visitedEdges.begin(); edgeIt != visitedEdges.end(); ++edgeIt)
			{
				qhHalfEdge& edge = **edgeIt;

				UpdatePointer(edge.pPrev, pEdge, pNew);
				UpdatePointer(edge.pNext, pEdge, pNew);
				UpdatePointer(edge.pTwin, pEdge, pNew);
			}

			for (int edgeIndex = 0; edgeIndex < edgeCount; ++edgeIndex)
			{
				qhHalfEdge& edge = edgeArray[edgeIndex];

				UpdatePointer(edge.pPrev, pEdge, pNew);
				UpdatePointer(edge.pNext, pEdge, pNew);
				UpdatePointer(edge.pTwin, pEdge, pNew);
			}

			for (auto faceIt = visitedFaces.begin(); faceIt != visitedFaces.end(); ++faceIt)
			{
				qhFace& face = **faceIt;

				UpdatePointer(face.pEdge, pEdge, pNew);
			}

			++i;
		}
	}

	// Move all faces
	{
		auto UpdatePointer = [](qhFace*& pointer, qhFace* pTest, qhFace* pSet)
		{
			if (pointer == pTest)
			{
				pointer = pSet;
			}
		};

		int i = 0;

		for (auto it = visitedFaces.begin(); it != visitedFaces.end(); ++it)
		{
			qhFace* pFace = *it;

			assert(i < faceCount);
			faceArray[i] = *pFace;

			qhFace* pNew = &faceArray[i];

			// Update pointers to this edge
			for (int edgeIndex = 0; edgeIndex < edgeCount; ++edgeIndex)
			{
				qhHalfEdge& edge = edgeArray[edgeIndex];

				UpdatePointer(edge.pFace, pFace, pNew);
			}

			++i;
		}
	}

	delete[] this->edges;
	delete[] this->verts;
	delete[] this->faces;

	this->edges = edgeArray;
	this->faces = faceArray;
	this->verts = vertArray;
}

ConvexHull::ConvexHull(const int pointCount, const glm::vec3* vertices)
{
	edgeCount = faceCount = vertCount = 0;
	epsilon = sqrEpsilon = 0;
	edges = nullptr;
	faces = nullptr;
	verts = nullptr;

	QuickHull(pointCount, vertices);
}

ConvexHull::~ConvexHull()
{
	delete[] this->edges;
	delete[] this->verts;
	delete[] this->faces;
}