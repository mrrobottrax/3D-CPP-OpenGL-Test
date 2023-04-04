#include <pch.h>
#include <physics/convexhull.h>

#include <debugtools/debugdraw.h>
#include <unordered_set>

#ifdef DEBUG
	void DrawPolygonEdges(qhHalfEdge& startEdge)
	{
		int i = 0;
		qhHalfEdge* edge = &startEdge;
		glm::vec3 add(0);
		// Explode faces a little
		if (startEdge.face)
		{
			add = startEdge.face->normal * 0.02f;
		}
		do {
			glm::vec3 start = { (*edge).tail->position[0], (*edge).tail->position[1], (*edge).tail->position[2] };
			glm::vec3 end = { (*edge).next->tail->position[0], (*edge).next->tail->position[1], (*edge).next->tail->position[2] };
			debugDraw.DrawLine(start + add, end + add, {std::fmodf(i / 2.1f, 1), 0, 0}, FLT_MAX);

			edge = edge->next;
			++i;
		} while (edge != &startEdge);
	}

	void DrawHullRecursive(qhHalfEdge& startEdge, std::unordered_set<qhHalfEdge*>& visited)
	{
		qhHalfEdge* edge = &startEdge;
		do {
			// Only recurse when the edge is not already in the list
			if (visited.find(edge) == visited.end())
			{
				visited.insert(edge);
				DrawHullRecursive(*(edge->twin), visited);
			}

			edge = edge->next;
		} while (edge != &startEdge);
	}

	void DrawHull(qhHalfEdge& startEdge)
	{
		std::unordered_set<qhHalfEdge*> visited;

		DrawHullRecursive(startEdge, visited);

		for (auto it = visited.begin(); it != visited.end(); ++it)
		{
			qhHalfEdge* edge = *it;

			glm::vec3 add(0);
			if (edge->face)
			{
				add = edge->face->normal * 0.02f;
			}

			glm::vec3 start = { (*edge).tail->position[0], (*edge).tail->position[1], (*edge).tail->position[2] };
			glm::vec3 end = { (*edge).next->tail->position[0], (*edge).next->tail->position[1], (*edge).next->tail->position[2] };
			debugDraw.DrawLine(start + add, end + add, { 1, 0, 1 }, FLT_MAX);
		}
	}
#endif // DEBUG

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
				std::cout << "DUPLICATE VERTICES DETECTED AND REMOVED: "
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

void ConvexHull::InitialHull(const std::list<glm::vec3>& vertices)
{
	//Create initial hull with min and max verts in each axis
	glm::vec3 bounds[6] = { glm::vec3(FLT_MAX), glm::vec3(FLT_MAX), glm::vec3(FLT_MAX), glm::vec3(FLT_MIN), glm::vec3(FLT_MIN), glm::vec3(FLT_MIN) };
	{
		glm::vec3* mins = &bounds[0];
		glm::vec3* maxs = &bounds[3];

		for (auto it = vertices.begin(); it != vertices.end(); ++it)
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
	glm::vec3 hullVerts[4] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
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
					hullVerts[0] = vertA;
					hullVerts[1] = vertB;
				}
			}
		}
	}

	// Get furthest vert from line
	{
		float bestDist = 0;
		for (auto it = vertices.begin(); it != vertices.end(); ++it)
		{
			float dist = DistFromLine(hullVerts[0], hullVerts[1], *it);
			if (dist >= bestDist)
			{
				hullVerts[2] = *it;
				bestDist = dist;
			}
		}
	}

	// Get furthest vert from plane
	Plane basePlane = PlaneFromTri(hullVerts[0], hullVerts[1], hullVerts[2]);
	bool planeFlipped = false;
	{
		float bestDist = 0;
		for (auto it = vertices.begin(); it != vertices.end(); ++it)
		{
			float dist = DistFromPlane(basePlane, *it);
			if (dist >= bestDist)
			{
				hullVerts[3] = *it;
				bestDist = dist;
			}
		}

		// Flip plane if needed
		if (glm::dot(basePlane.normal, hullVerts[3]) - std::abs(basePlane.dist) >= 0)
		{
			basePlane.Invert();
			planeFlipped = true;
		}
	}

	// Add vertices to hull
	qhVertex* baseHullVerts[4];
	for (int i = 0; i < 4; ++i)
	{
		qhVertex vertex = {
			nullptr,

			{ hullVerts[i][0], hullVerts[i][1], hullVerts[i][2] }
		};
		this->workingVertices.push_back(vertex);
		baseHullVerts[i] = &workingVertices.back();
	}

	// Construct initial hull
	// Create base
	qhHalfEdge* baseEdges[3];
	if (!planeFlipped)
	{
		workingEdges.push_back({
			&workingVertices[0]
			});
		baseEdges[0] = &workingEdges.back();

		workingEdges.push_back({
			&workingVertices[1]
			});
		baseEdges[1] = &workingEdges.back();

		workingEdges.push_back({
			&workingVertices[2]
			});
		baseEdges[2] = &workingEdges.back();
	}
	else // Swap order of vertices to make face counter clockwise
	{
		workingEdges.push_back({
			&workingVertices[2]
			});
		baseEdges[0] = &workingEdges.back();

		workingEdges.push_back({
			&workingVertices[1]
			});
		baseEdges[1] = &workingEdges.back();

		workingEdges.push_back({
			&workingVertices[0]
			});
		baseEdges[2] = &workingEdges.back();
	}

	ConnectEdgeLoop(baseEdges, 3);

	// Connect edges to face
	workingFaces.push_back({
		baseEdges[0],

		basePlane.normal,
		basePlane.dist,
	});
	for (int i = 0; i < 3; ++i)
	{
		baseEdges[i]->face = &workingFaces.back();
	}

	// Connect base to vertex
	// baseEdges flows clockwise from the perspective of the eye which is unacceptable
	// Reverse baseEdges
	{
		qhHalfEdge* temp = baseEdges[0];

		baseEdges[0] = baseEdges[2];
		baseEdges[2] = temp;
	}
	AddPoint(baseEdges, 3, *baseHullVerts[3]);

	DrawHull(*baseEdges[0]);
}

void ConvexHull::AddPoint(qhHalfEdge** horizon, const int horizonSize, qhVertex& eye)
{
	qhHalfEdge* loopEdge = nullptr; // The edge that the loop eventually has to connect back to
	qhHalfEdge* connectingEdge = nullptr;
	qhHalfEdge* twinEdge = horizon[0];
	for (int i = 0; i < horizonSize; ++i)
	{
		// If we exhaust the horizon, loop back to the start
		qhHalfEdge* nextEdge;
		if (i + 1 >= horizonSize)
		{
			nextEdge = horizon[0];
		}
		else
		{
			nextEdge = horizon[i + 1];
		}
		
		qhHalfEdge* edges[3];

		// Create edges
		this->workingEdges.push_back({
			twinEdge->tail
			});
		edges[0] = &workingEdges.back();
		this->workingEdges.push_back({
			nextEdge->tail
			});
		edges[1] = &workingEdges.back();
		this->workingEdges.push_back({
			&eye
			});
		edges[2] = &workingEdges.back();

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

		edges[0]->twin = twinEdge;
		twinEdge->twin = edges[0];

		// Create face
		Plane plane = PlaneFromTri(edges[0]->tail->position, edges[1]->tail->position, edges[2]->tail->position);
		this->workingFaces.push_back({
			edges[0],

			plane.normal,
			plane.dist,
		});

		// Add face reference to edges
		for (int i = 0; i < 3; ++i)
		{
			edges[i]->face = &this->workingFaces.back();
		}

		connectingEdge = edges[1];
		twinEdge = nextEdge;
	}

	// Merge back into the loop
	// C6011 false positive
#pragma warning( push )
#pragma warning( disable : 6011)
	connectingEdge->twin = loopEdge;
	loopEdge->twin = connectingEdge;
#pragma warning( pop )
}

void ConvexHull::QuickHull(const int vertCount, const glm::vec3* verticesArray)
{
	std::list<glm::vec3> vertices;
	std::copy(verticesArray, &verticesArray[vertCount], std::back_inserter(vertices));

	epsilon = CalcEpsilon(vertices);
	sqrEpsilon = epsilon * epsilon;

	// TODO: Might not even help at all
	RemoveDuplicateVertices(vertices);

	InitialHull(vertices);

	// Parition vertices

}

ConvexHull::ConvexHull(const int vertCount, const glm::vec3* vertices)
{
	QuickHull(vertCount, vertices);
}