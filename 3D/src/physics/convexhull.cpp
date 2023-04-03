#include <pch.h>
#include <physics/convexhull.h>

void ConvexHull::QuickHull(int vertCount, Vertex* vertices)
{
	std::vector<Vertex> verticesList;

	for (int i = 0; i < vertCount; ++i)
	{
		verticesList.push_back(vertices[i]);
	}

	//// Calculate epsilon based on bounds
	//float epsilon;
	//{
	//	float min[3] = { 0, 0, 0 };
	//	float max[3] = { 0, 0, 0 };

	//	for (auto it = verticesList.begin(); it != verticesList.end(); ++it)
	//	{
	//		for (int i = 0; i < 3; ++i)
	//		{
	//			min[i] = std::min(min[i], (*it).position[i]);
	//		}

	//		for (int i = 0; i < 3; ++i)
	//		{
	//			max[i] = std::max(max[i], (*it).position[i]);
	//		}
	//	}

	//	std::cout << "MIN: " << min[0] << ", " << min[1] << ", " << min[2] << "\n";
	//	std::cout << "MAX: " << max[0] << ", " << max[1] << ", " << max[2] << "\n";
	//}
}

ConvexHull::ConvexHull(int vertCount, Vertex* vertices)
{
	QuickHull(vertCount, vertices);
}

ConvexHull::~ConvexHull()
{
	delete[] vertices;
	delete[] halfEdges;
	delete[] faces;
}