#pragma once
#include <glm/glm.hpp>
#include <vector>

/**
	Referece material used: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/rtcd.pdf
**/

namespace Collision
{
	struct Collider{};
	struct Plane : Collider
	{
		glm::vec3 normal; //normalized
		float d; // d = dot(n,p)
		Plane(glm::vec3 normal, float d) : normal{ normal }, d {d} {}
	};

	struct AABB : Collider
	{
		glm::vec3 min;
		glm::vec3 max;
		AABB(glm::vec3 min, glm::vec3 max) : min{ min }, max{ max } {}
		float GetSurfaceArea();
	};

	struct Ray : Collider
	{
		glm::vec3 start;
		glm::vec3 direction;
		Ray(glm::vec3 start, glm::vec3 dir) : start{ start }, direction{ dir } {}
	};

	// Check Collision Function
	bool PointAABB(const glm::vec3& p, const AABB& a);
	bool PointPlane(const glm::vec3& p, const Plane& t);

	bool RayPlane(const Ray& ray, const Plane& p, float& t, glm::vec3& q);
	bool RayAABB(const Ray& r, const AABB& a, float& tmin, glm::vec3& q);
	bool SegmentAABB(const glm::vec3& p0, const glm::vec3& p1, const AABB& b);

	bool AABBPlane(const AABB& b, const Plane& p);
	bool AABBAABB(const AABB& a, const AABB& b);

	glm::vec3 ClosestPtPointAABB(glm::vec3 p, AABB b);

};