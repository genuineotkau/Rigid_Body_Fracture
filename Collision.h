#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Collision
{
	struct Collider{};
	struct Plane : Collider
	{
		glm::vec3 normal; //normalized
		float d; // d = dot(n,p)
		Plane(glm::vec3 normal, float d) : normal{ normal }, d {d} {}
	};

	struct Triangle : Collider
	{
		glm::vec3 v1;
		glm::vec3 v2;
		glm::vec3 v3;
		Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) : v1 {v1}, v2 {v2}, v3 {v3} {}
	};

	struct Sphere : Collider
	{
		glm::vec3 position;
		float radius;
		Sphere(glm::vec3 pos, float radius) : position{ pos }, radius{ radius } {};
		float GetSurfaceArea();
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
	bool PointSphere(const glm::vec3& p, const Sphere& S);
	bool PointAABB(const glm::vec3& p, const AABB& a);
	bool PointPlane(const glm::vec3& p, const Plane& t);
	bool PointTriangle(const glm::vec3& p, const Triangle& t);

	bool RayPlane(const Ray& ray, const Plane& p, float& t, glm::vec3& q);
	bool RaySphere(const Ray& r, const Sphere& s, float& t, glm::vec3& q);
	bool RayAABB(const Ray& r, const AABB& a, float& tmin, glm::vec3& q);
	bool SegmentAABB(const glm::vec3& p0, const glm::vec3& p1, const AABB& b);
	bool RayTriangle(glm::vec3& p, glm::vec3& q, glm::vec3& a, glm::vec3& b, glm::vec3& c,
		float& u, float& v, float& w, float& t, glm::vec3& result);

	bool SphereSphere(const Sphere& a, const Sphere& b);
	bool SphereAABB(const Sphere& a, const AABB& b);
	bool SpherePlane(const Sphere& s, const Plane& p);
	bool SphereTriangle(const Sphere& s, const Triangle& t);

	bool AABBPlane(const AABB& b, const Plane& p);
	bool AABBAABB(const AABB& a, const AABB& b);
	bool AABBTriangle(const AABB& b, Triangle& t);

	glm::vec3 ClosestPtPointAABB(glm::vec3 p, AABB b);

};