#include <pch.h>
#include <physics/convexhull.h>

ConvexHull::~ConvexHull()
{
	delete[] faces;
	delete[] vertices;
}