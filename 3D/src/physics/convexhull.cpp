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
	qhHalfEdge* edge = &startEdge;
	glm::vec3 add(0);
	// Explode faces a little
	if (startEdge.face)
	{
		add = startEdge.face->plane.normal * explode;
	}
	do {
		glm::vec3 start = { (*edge).tail->position[0], (*edge).tail->position[1], (*edge).tail->position[2] };
		glm::vec3 end = { (*edge).next->tail->position[0], (*edge).next->tail->position[1], (*edge).next->tail->position[2] };

		debugDraw.DrawLine(start + add, end + add, color, time);

		edge = edge->next;
		++i;
	} while (edge != &startEdge);
}

void DrawHull(const qhFace& startFace, const float time = FLT_MAX, const bool points = false, const float normalDist = 0)
{
	std::unordered_set<qhFace*> visited;

	// Depth first search
	std::function<void(const qhFace&)> DrawHullRecursive = [&](const qhFace& face)
	{
		qhHalfEdge& startEdge = *face.edge;
		qhHalfEdge* edge = &startEdge;
		do {
			// Only recurse when the edge is not already in the list
			if (visited.find(edge->twin->face) == visited.end())
			{
				visited.insert(edge->twin->face);

				if (edge->twin != nullptr && edge->twin->face != nullptr)
				{
					DrawHullRecursive(*edge->twin->face);
				}
				else
				{
					printf("QHull Error: Drawing bad face");
					DrawPolygonEdges(*edge, { 1, 1, 0 }, 60, 0.05f);
					debugDraw.DrawLine(edge->tail->position, edge->next->tail->position, { 0, 0, 1 }, 60);
					std::this_thread::sleep_for(std::chrono::seconds(120));
				}

			}

			edge = edge->next;
		} while (edge != &startEdge);
	};

	DrawHullRecursive(startFace);

	// Draw each face
	for (auto it = visited.begin(); it != visited.end(); ++it)
	{
		glm::vec3 color{0, 1, 1};
			
		glm::vec3 center(0);
		int divide = 0;

		glm::vec3 add = (*it)->plane.normal * 0.02f;

		qhHalfEdge& startEdge = *(*it)->edge;
		qhHalfEdge* edge = &startEdge;
		do {
			debugDraw.DrawLine(edge->tail->position + add, edge->next->tail->position + add, color, time);

			center += edge->tail->position;
			++divide;

			edge = edge->next;
		} while (edge != &startEdge);

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
			edges[j]->prev = edges[edgeCount - 1];
		else
			edges[j]->prev = edges[(j - 1) % edgeCount];

		edges[j]->next = edges[(j + 1) % edgeCount];
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
			qhFace* bestFace = nullptr;
			for (int i = 0; i < newFaces.size(); ++i)
			{
				qhFace& conflictFace = *newFaces[i];
				float dist = SignedDistFromPlane(conflictFace.plane, point);
				if (dist >= 0)
					dist -= epsilon;

				if (dist <= bestDist && dist > 0)
				{
					bestFace = &conflictFace;
				}
			}

			// Point is outside
			if (bestFace)
			{
				bestFace->conflictList.push_back(point);
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

		qhHalfEdge& startEdge = *b.edge;
		qhHalfEdge* edge = &startEdge;
		do {
			bCenter += edge->tail->position;

			edge = edge->next;
			++divide;
		} while (edge != &startEdge);

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
		qhHalfEdge* edge = horizon[i];
		qhFace& face1 = *edge->twin->face;
		qhFace& face2 = *edge->face;

		if (!IsCoplanar(face1, face2))
			continue;

		// ~~~~~~~~~~~~~~~~~Merge faces ~~~~~~~~~~~~~~~~~

		// Absorb conflict points
		// face 1 absorbs face 2 so that continuous face merging works
		for (int i = 0; i < face2.conflictList.size(); ++i)
			face1.conflictList.push_back(face2.conflictList[i]);

		// Set face references in edges
		{
			qhHalfEdge& startEdge2 = *face2.edge;
			qhHalfEdge* edge2 = &startEdge2;
			do {
				edge2->face = &face1;

				edge2 = edge2->next;
			} while (edge2 != &startEdge2);
		}

		qhHalfEdge& seperatingEdgeANext = *edge->twin->next;
		qhHalfEdge& seperatingEdgeAPrev = *edge->twin->prev;

		qhHalfEdge& seperatingEdgeBNext = *edge->next;
		qhHalfEdge& seperatingEdgeBPrev = *edge->prev;

		// Set edge reference in face
		face1.edge = &seperatingEdgeANext;

		// Connect edges
		seperatingEdgeAPrev.next = &seperatingEdgeBNext;
		seperatingEdgeANext.prev = &seperatingEdgeBPrev;

		seperatingEdgeBPrev.next = &seperatingEdgeANext;
		seperatingEdgeBNext.prev = &seperatingEdgeAPrev;

		// Remove old stuff
		unvisitedFaces.erase(&face2);
		RemoveFace(face2);
		RemoveEdge(*edge->twin);
		RemoveEdge(*edge);

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
		baseVerts[0]->edge = baseEdges[0] = AddEdge();
		baseEdges[0]->tail = baseVerts[0];
		
#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[0]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		baseVerts[1]->edge = baseEdges[1] = AddEdge();
		baseEdges[1]->tail = baseVerts[1];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[1]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		baseVerts[2]->edge = baseEdges[2] = AddEdge();
		baseEdges[2]->tail = baseVerts[2];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[2]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

	}
	else // Swap order of vertices to make face counter clockwise
	{
		baseVerts[2]->edge = baseEdges[0] = AddEdge();
		baseEdges[0]->tail = baseVerts[2];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[0]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		baseVerts[1]->edge = baseEdges[1] = AddEdge();
		baseEdges[1]->tail = baseVerts[1];

#ifdef QHULL_DEBUG
		DrawPoint(baseEdges[1]->tail->position, { 1, 1, 1 }, delayTest);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		baseVerts[0]->edge = baseEdges[2] = AddEdge();
		baseEdges[2]->tail = baseVerts[0];

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
		tetrahedronFaces[0]->edge = baseEdges[0];
		tetrahedronFaces[0]->plane = {
				basePlane.normal,
				basePlane.dist
		};

		for (int i = 0; i < 3; ++i)
		{
			baseEdges[i]->face = tetrahedronFaces[0];
		}
	}

	// Reverse edge order for horizon to be counter clockwise
	{
		qhHalfEdge* temp = baseEdges[0];
		baseEdges[0] = baseEdges[1];
		baseEdges[1] = temp;
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
		qhFace* bestFace = nullptr;
		for (auto f : tetrahedronFaces)
		{
			float dist = SignedDistFromPlane(f->plane, vertex);
			if (dist >= 0)
				dist -= epsilon;

			if (dist <= bestDist && dist > 0)
			{
				bestFace = f;
			}
		}

		// Vertex is outside
		if (bestFace)
		{
			bestFace->conflictList.push_back(vertex);
		}
	}
}

void ConvexHull::AddPoint(qhHalfEdge** horizon, const int horizonSize, glm::vec3& eye, std::vector<qhFace*>& newFaces)
{
	// Create vertex
	qhVertex& eyeVert = *AddVertex();
	eyeVert.position = eye;

	qhHalfEdge* loopEdge = nullptr; // The edge that the loop eventually has to connect back to
	qhHalfEdge* connectingEdge = nullptr;
	for (int i = 0; i < horizonSize; ++i)
	{
		qhHalfEdge& edge = *horizon[i];
		qhHalfEdge& nextEdge = *edge.next;

		qhHalfEdge* edges[3]{};

		// Create edges
		edges[0] = AddEdge();
		edges[0]->tail = nextEdge.tail;

		edges[1] = AddEdge();
		edges[1]->tail = edge.tail;

		edges[2] = AddEdge();
		edges[2]->tail = &eyeVert;

		Plane plane = PlaneFromTri(edges[0]->tail->position, edges[1]->tail->position, edges[2]->tail->position);

		if (connectingEdge != nullptr)
		{
			edges[2]->twin = connectingEdge;
			connectingEdge->twin = edges[2];
		}
		else // We are on the first face
		{
			loopEdge = edges[2];
		}

		ConnectEdgeLoop(edges, 3);

		edges[0]->twin = &edge;
		edge.twin = edges[0];

		// Create face
		newFaces.push_back(AddFace());
		newFaces.back()->edge = edges[0];
		newFaces.back()->plane = {
			plane.normal,
			plane.dist
		};

		// Add face reference to edges
		for (int i = 0; i < 3; ++i)
		{
			edges[i]->face = newFaces.back();
		}

		connectingEdge = edges[1];
	}

// False positive
// C6011: Dereferencing NULL pointer
#pragma warning( push )
#pragma warning( disable : 6011 )
	connectingEdge->twin = loopEdge;
	loopEdge->twin = connectingEdge;
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
	qhFace* lastFace = nullptr;
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
		glm::vec3* eye = nullptr;
		{
			float bestDist = 0;
			for (int p = 0; p < face.conflictList.size(); ++p)
			{
				glm::vec3& point = face.conflictList[p];

				float dist = gMath::SignedDistFromPlane(face.plane, point);
				if (dist >= bestDist)
				{
					bestDist = dist;
					eye = &point;
				}
			}

			if (eye == nullptr)
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
				qhFace& face = *startEdge.face;

				visible.insert(&face);

				// Recurse on face edges
				{
					qhHalfEdge* edge = &startEdge;
					do
					{
						qhFace& twinFace = *edge->twin->face;

						bool twinVisible = FaceIsVisible(twinFace, eye);

						if (visible.find(&twinFace) == visible.end() && twinVisible)
						{
							// Face has not been visited
							FindVisibleRecursive(*edge->twin, eye);
						}

						if (!twinVisible)
						{
							horizon.push_back(edge->twin);
						}

						edge = edge->next;
					} while (edge != &startEdge);
				}
			};

			FindVisibleRecursive(*face.edge, *eye);

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
		AddPoint(&horizon[0], (int)horizon.size(), *eye, newFaces);

		for (auto f : newFaces)
		{
			unvisitedFaces.insert(f);
		}

#ifdef QHULL_DEBUG
		DrawHull(**newFaces.begin(), delayTest, true, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		// Repartition all old face conflict vertices
		RePartitionVertices(visible, newFaces, *eye);

		// Merge coplanar faces
		MergeCoplanar(horizon, unvisitedFaces);

#ifdef QHULL_DEBUG
		DrawHull(**newFaces.begin(), delayTest, true, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds((long)(1000 * delayTest)));
#endif // QHULL_DEBUG

		// Remove obscured faces
		for (auto it = visible.begin(); it != visible.end(); ++it)
		{
			qhHalfEdge& startEdge = *(*it)->edge;
			qhHalfEdge* edge = &startEdge;
			do {
				qhHalfEdge* edge2 = edge;

				edge = edge->next;

				RemoveEdge(*edge2);
			} while (edge != &startEdge);

			unvisitedFaces.erase(*it);
			RemoveFace(**it);
		}

		lastFace = newFaces[0];
	}

	//CondenseArrays(*lastFace);

#ifdef QHULL_DEBUG
	if (lastFace != nullptr)
		DrawHull(*lastFace, 120, false, 0.5f);
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

// TODO: Fix this
// It doesn't work because it copies pointers over as well
// Also the code has been updated and the arrays are pretty dense already
void ConvexHull::CondenseArrays(qhFace& startFace)
{
	std::unordered_set<qhFace*> visitedFaces;
	std::unordered_set<qhHalfEdge*> visitedEdges;
	std::unordered_set<qhVertex*> visitedVertices;

	std::function<void(qhFace&)> GetDataRecursive = [&](qhFace& startFace)
	{
		qhHalfEdge& startEdge = *startFace.edge;
		qhHalfEdge* edge = &startEdge;
		do {
			visitedEdges.insert(edge);
			visitedVertices.insert(edge->tail);

			// Only recurse when the face is not already in the list
			if (visitedFaces.find(edge->twin->face) == visitedFaces.end())
			{
				visitedFaces.insert(edge->twin->face);
				GetDataRecursive(*(edge->twin->face));
			}

			edge = edge->next;
		} while (edge != &startEdge);
	};

	GetDataRecursive(startFace);

	vertCount = visitedVertices.size();
	edgeCount = visitedEdges.size();
	faceCount = visitedFaces.size();

	qhVertex* vertArray = new qhVertex[vertCount];
	memset(vertArray, 0, sizeof(qhVertex) * vertCount);

	qhHalfEdge* edgeArray = new qhHalfEdge[edgeCount];
	memset(edgeArray, 0, sizeof(qhHalfEdge) * edgeCount);

	qhFace* faceArray = new qhFace[faceCount];
	memset(faceArray, 0, sizeof(qhFace) * faceCount);

	auto FindInEdges = [&](qhHalfEdge* edge) -> qhHalfEdge*
	{
		if (edge == nullptr)
			return nullptr;

		int placeIndex = -1;
		for (int i = 0; i < edgeCount; ++i)
		{
			char testblock[sizeof(qhHalfEdge)];
			memset(testblock, 0, sizeof testblock);

			if (memcmp(&edgeArray[i], testblock, sizeof(qhHalfEdge)))
			{
				placeIndex = i;
			}

			if (memcmp(&edgeArray[i], edge, sizeof(qhHalfEdge)))
			{
				return &edgeArray[i];
			}
		}

		edgeArray[placeIndex] = *edge;
		return &edgeArray[placeIndex];
	};
	auto FindInVerts = [&](qhVertex* vert) -> qhVertex*
	{
		if (vert == nullptr)
			return nullptr;

		int placeIndex = 0;
		for (int i = 0; i < vertCount; ++i)
		{
			char testblock[sizeof(qhVertex)];
			memset(testblock, 0, sizeof testblock);

			if (memcmp(&vertArray[i], testblock, sizeof(qhVertex)))
			{
				placeIndex = i;
			}

			if (memcmp(&vertArray[i], vert, sizeof(qhVertex)))
			{
				return &vertArray[i];
			}
		}

		vertArray[placeIndex] = *vert;
		return &vertArray[placeIndex];
	};
	auto FindInFaces = [&](qhFace* face) -> qhFace*
	{
		if (face == nullptr)
			return nullptr;

		int placeIndex = 0;
		for (int i = 0; i < faceCount; ++i)
		{
			char testblock[sizeof(qhFace)];
			memset(testblock, 0, sizeof testblock);

			if (memcmp(&faceArray[i], testblock, sizeof(qhFace)))
			{
				placeIndex = i;
			}

			if (memcmp(&faceArray[i], face, sizeof(qhFace)))
			{
				return &faceArray[i];
			}
		}

		faceArray[placeIndex] = *face;
		return &faceArray[placeIndex];
	};

	for (auto it = visitedVertices.begin(); it != visitedVertices.end(); ++it)
	{
		qhVertex& vert = **it;

		vert.edge = FindInEdges(vert.edge);
	}
	for (auto it = visitedEdges.begin(); it != visitedEdges.end(); ++it)
	{
		qhHalfEdge& edge = **it;

		edge.face = FindInFaces(edge.face);

		edge.next = FindInEdges(edge.next);
		edge.prev = FindInEdges(edge.prev);
		edge.twin = FindInEdges(edge.twin);

		edge.tail = FindInVerts(edge.tail);
	}
	for (auto it = visitedFaces.begin(); it != visitedFaces.end(); ++it)
	{
		qhFace& face = **it;

		face.edge = FindInEdges(face.edge);
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