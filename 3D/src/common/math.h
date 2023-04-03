#pragma once

struct Plane
{
	glm::vec3 normal;
	float dist;
};

float SqrMagnitude(glm::vec3 vector);
float SqrDist(glm::vec3 vertexA, glm::vec3 vertexB);
float DistFromLine(glm::vec3 linePointA, glm::vec3 linePointB, glm::vec3 point);