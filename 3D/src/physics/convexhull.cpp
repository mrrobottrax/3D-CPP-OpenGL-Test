#include <pch.h>
#include <physics/convexhull.h>

ConvexHull::ConvexHull(int planeCount, Plane* planes)
{
	//TODO: This is really naive way of doing this

	faces = new Face[planeCount];
	for (int i = 0; i < planeCount; ++i)
	{
		faces[i] = {
			nullptr,
			planes[i].normal,
			planes[i].dist,
		};
	}

	for (int i = 0; i < planeCount; ++i)
	{
		Face& face = faces[i];
		std::list<Vertex> vertices;
		std::list<Edge> edges;
		std::list<HalfEdge> halfEdges;

		for (int j = 0; j < planeCount; ++j)
		{
			if (j == i)
				continue;

			Face& clipFace = faces[j];

			// Early exit with same or opposite normals
			if (clipFace.normal == face.normal || clipFace.normal == -face.normal)
				continue;

			// Intersecting planes create a vertex
		}
	}
}

ConvexHull::~ConvexHull()
{
	delete[] vertices;
	delete[] halfEdges;
	delete[] edges;
	delete[] faces;
}