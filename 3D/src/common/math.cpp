#include <pch.h>
#include <common/math.h>

#include <glm.hpp>

float SqrMagnitude(glm::vec3 vec)
{
	float sqrMagnitude = 0;
	for (int i = 0; i < 3; ++i)
	{
		sqrMagnitude += vec[i] * vec[i];
	}

	return sqrMagnitude;
};

float SqrDist(glm::vec3 vertexA, glm::vec3 vertexB)
{
	return SqrMagnitude(vertexA - vertexB);
}