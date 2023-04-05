#pragma once

namespace gMath
{
	struct Plane
	{
		glm::vec3 normal;
		float dist;

		void Invert()
		{
			normal = -normal;
			dist = -dist;
		}
	};

	float SqrMagnitude(glm::vec3 vector);
	float SqrDist(glm::vec3 vertexA, glm::vec3 vertexB);
	float DistFromLine(glm::vec3 linePointA, glm::vec3 linePointB, glm::vec3 point);
	float SignedDistFromPlane(Plane plane, glm::vec3 point);
	float SignedDistFromTriPlane(glm::vec3 triPointA, glm::vec3 triPointB, glm::vec3 triPointC, glm::vec3 point);
	Plane PlaneFromTri(glm::vec3 triPointA, glm::vec3 triPointB, glm::vec3 triPointC);
	Plane PlaneFromTri(const float triPointA[3], const float triPointB[3], const float triPointC[3]);
}