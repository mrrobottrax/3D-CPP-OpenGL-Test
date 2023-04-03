#include <pch.h>
#include <common/math.h>

#include <glm.hpp>

float SqrMagnitude(const glm::vec3 vec)
{
	float sqrMagnitude = 0;
	for (int i = 0; i < 3; ++i)
	{
		sqrMagnitude += vec[i] * vec[i];
	}

	return sqrMagnitude;
};

float SqrDist(const glm::vec3 vertexA, const glm::vec3 vertexB)
{
	return SqrMagnitude(vertexA - vertexB);
}

float DistFromLine(const glm::vec3 linePointA, const glm::vec3 linePointB, const glm::vec3 point)
{
	float dist = glm::length(glm::cross(point - linePointA, point - linePointB));
	dist /= glm::length(linePointB - linePointA);

	return dist;
}

float DistFromPlane(const Plane plane, const glm::vec3 point)
{
	return glm::dot(plane.normal, point) - plane.dist;
}

float DistFromTriPlane(const glm::vec3 triPointA, const glm::vec3 triPointB, const glm::vec3 triPointC, const glm::vec3 point)
{
	Plane plane = PlaneFromTri(triPointA, triPointB, triPointC);

	return DistFromPlane(plane, point);
}

Plane PlaneFromTri(const glm::vec3 triPointA, const glm::vec3 triPointB, const glm::vec3 triPointC)
{
	Plane plane;

	plane.normal = glm::cross(triPointA - triPointB, triPointB - triPointC);
	plane.dist = 0;

	plane.dist += glm::dot(plane.normal, triPointA);
	plane.dist += glm::dot(plane.normal, triPointB);
	plane.dist += glm::dot(plane.normal, triPointC);

	plane.dist /= 3;

	return plane;
}

Plane PlaneFromTri(const float triPointA[3], const float triPointB[3], const float triPointC[3])
{
	return PlaneFromTri((glm::vec3)triPointA[0], (glm::vec3)triPointB[0], (glm::vec3)triPointC[0]);
}
