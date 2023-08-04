#include "BoundingVolume.h"

namespace BoundingVolume
{

	Collision::AABB createAABB(std::vector<glm::vec3>& vertices)
	{
		glm::vec3 Min = glm::vec3(FLT_MAX);
		glm::vec3 Max = glm::vec3(-FLT_MAX);
		for (unsigned int i = 0; i < vertices.size(); ++i)
		{
			glm::vec3 pt = vertices.at(i);
			Min.x = std::min(Min.x, vertices[i].x);
			Min.y = std::min(Min.y, vertices[i].y);
			Min.z = std::min(Min.z, vertices[i].z);
			Max.x = std::max(Max.x, vertices[i].x);
			Max.y = std::max(Max.y, vertices[i].y);
			Max.z = std::max(Max.z, vertices[i].z);
		}
		return Collision::AABB(Min, Max);
	}

	glm::vec3 getExtents(RigidBody* obj)
	{
		glm::vec3 result = (obj->aabb.max - obj->aabb.min) / 2.0f;
		return result;
	}
}
