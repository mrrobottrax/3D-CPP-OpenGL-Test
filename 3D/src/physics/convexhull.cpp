#include <pch.h>
#include <physics/convexhull.h>

ConvexHull::ConvexHull(int vertCount, Vertex* vertices)
{
	
}

ConvexHull::~ConvexHull()
{
	delete[] vertices;
	delete[] halfEdges;
	delete[] faces;
}