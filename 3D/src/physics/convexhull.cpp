#include <pch.h>
#include <physics/convexhull.h>

#include <debugtools/debugdraw.h>

#ifdef DEBUG
void DrawPolygonEdges(qhHalfEdge& startEdge)
{
	qhHalfEdge* edge = &startEdge;
	do {
		debugDraw.DrawLine((*edge).tail->position, (*edge).next->tail->position, {1, 0, 1}, 10);
		edge = edge->next;
	} while (edge != &startEdge);
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
	}

	// Add vertices to hull
	for (int i = 0; i < 4; ++i)
	{
		qhVertex vertex = {
			nullptr,
			nullptr,

			nullptr,

			{ hullVerts[i][0], hullVerts[i][1], hullVerts[i][2] }
		};
		this->workingVertices.push_back(vertex);
	}

	// Construct initial hull
	// Create base
	// Vertices
	this->workingVertices[0].next = &this->workingVertices[1];
	this->workingVertices[1].prev = &this->workingVertices[0];

	this->workingVertices[1].next = &this->workingVertices[2];
	this->workingVertices[2].prev = &this->workingVertices[1];

	this->workingVertices[2].next = &this->workingVertices[0];
	this->workingVertices[0].next = &this->workingVertices[2];

	// Edges
	qhHalfEdge* firstEdge;
	qhHalfEdge* secondEdge;
	qhHalfEdge* thirdEdge;

	this->workingEdges.push_back({
		&workingVertices[0],

		nullptr,
		nullptr,
		nullptr,

		nullptr
		});
	firstEdge = &workingEdges.back();

	this->workingEdges.push_back({
		&workingVertices[1],

		firstEdge,
		nullptr,
		nullptr,

		nullptr
		});
	secondEdge = &workingEdges.back();

	this->workingEdges.push_back({
		&workingVertices[2],

		secondEdge,
		firstEdge,
		nullptr,

		nullptr
		});
	thirdEdge = &workingEdges.back();

	firstEdge->prev = thirdEdge;
	firstEdge->next = secondEdge;
	secondEdge->next = thirdEdge;

	this->workingFaces.push_back({
		nullptr,
		nullptr,

		firstEdge,

		basePlane.normal,
		basePlane.dist,
		});

	debugDraw.DrawPlane(glm::vec3(0), basePlane, 1, 1, { 1, 1, 1 }, 10);
	std::cout << basePlane.dist << "\n";

#ifdef DEBUG
	DrawPolygonEdges(*firstEdge);
#endif // DEBUG

	// Connect base to vertex
	for (int i = 0; i < 3; ++i)
	{
		//this->workingVertices[0] = 
	}
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
}

ConvexHull::ConvexHull(const int vertCount, const glm::vec3* vertices)
{
	QuickHull(vertCount, vertices);
}

ConvexHull::~ConvexHull()
{

}