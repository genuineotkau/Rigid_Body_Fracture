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

	std::pair<float, float> getExtents(RigidBody* obj, char axis)
	{
		std::pair<float, float> result;
		//if (renderSphere == false) // get AABB extents
		//{
			if (axis == 'x')
			{
				result.first = obj->position.x - obj->aabb.min.x;
				result.second = obj->position.x + obj->aabb.max.x;
			}
			else if (axis == 'y')
			{
				result.first = obj->position.y - obj->aabb.min.y;
				result.second = obj->position.y + obj->aabb.max.y;
			}
			else if (axis == 'z')
			{
				result.first = obj->position.z - obj->aabb.min.z;
				result.second = obj->position.z + obj->aabb.max.z;
			}
		//}
		return result;
	}
}
