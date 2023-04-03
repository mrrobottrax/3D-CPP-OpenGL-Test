#include <pch.h>
#include <physics/convexhull.h>

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
	glm::vec3 mins[3] = { glm::vec3(FLT_MAX), glm::vec3(FLT_MAX), glm::vec3(FLT_MAX) };
	glm::vec3 maxs[3] = { glm::vec3(FLT_MIN), glm::vec3(FLT_MIN), glm::vec3(FLT_MIN) };

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

	// Find two furthest vertices
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{

		}
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
	delete[] vertices;
	delete[] halfEdges;
	delete[] faces;
}