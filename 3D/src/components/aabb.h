#pragma once
#include <glm.hpp>

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;

    inline bool IsPointInside(const glm::vec3& p) const
    {
        const bool x = p.x >= min.x && p.x <= max.x;
        const bool y = p.y >= min.y && p.y <= max.y;
        const bool z = p.z >= min.z && p.z <= max.z;

        return x && y && z;
    };
};