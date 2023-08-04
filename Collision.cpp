#include "Collision.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>

/**
    Referece material used: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/rtcd.pdf
**/

#pragma region Point

bool Collision::PointAABB(const glm::vec3& p, const AABB& a)
{
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (p[i] < a.min[i] || p[i] > a.max[i])
			return false;
	}
	return true;
}

bool Collision::PointPlane(const glm::vec3& p, const Plane& t)
{
	if (glm::dot(t.normal, p) == t.d) return true;
	else return false;
}

#pragma endregion

#pragma region Ray
bool Collision::RayPlane(const Ray& ray, const Plane& p, float& t, glm::vec3& q)
{
	glm::vec3 ab = ray.direction;
	t = (p.d - glm::dot(p.normal, ray.start)) / glm::dot(p.normal, ab);

	if (t >= 0.0f) {
		q = t * ab;
		return true;
	}
	return false;
}

// using slab method to determine if ray intersects AABB
bool Collision::RayAABB(const Ray& r, const AABB& a, float& tmin, glm::vec3& q)
{
	tmin = 0.0f;
	float tmax = FLT_MAX;
	for (int i = 0; i < 3; i++) {
		if (std::abs(r.direction[i]) < FLT_EPSILON) {
			// ray is parallel
			if (r.start[i] < a.min[i] || r.start[i] > a.max[i]) return false;
		}
		else {
			// t1: near plane; t2: far plane
			float ood = 1.0f / r.direction[i];
			float t1 = (a.min[i] - r.start[i]) * ood;
			float t2 = (a.max[i] - r.start[i]) * ood;
			if (t1 > t2) std::swap(t1, t2);
			// compute intersection
			if (t1 > tmin) tmin = t1;
			if (t2 > tmax) tmax = t2;
			if (tmin > tmax) return false;
		}
	}
	tmin += FLT_EPSILON;
	q = r.direction * tmin;
	if (SegmentAABB(r.start, r.start + q, a)) return true;
	else return false;
}

bool Collision::SegmentAABB(const glm::vec3& p0, const glm::vec3& p1, const AABB& b)
{
	// box center point
	glm::vec3 c = (b.min + b.max) * 0.5f;
	// box halflength extents
	glm::vec3 e = b.max - c;
	// segment midpoint
	glm::vec3 m = (p0 + p1) * 0.5f;
	// segment halflength vector
	glm::vec3 d = p1 - m;
	// box and segment to origin
	m = m - c;
	float adx = std::abs(d.x);
	if (std::abs(m.x) > e.x + adx) return false;
	float ady = std::abs(d.y);
	if (std::abs(m.y) > e.y + ady) return false;
	float adz = std::abs(d.z);
	if (std::abs(m.z) > e.z + adz) return false;
	adx += FLT_EPSILON; ady += FLT_EPSILON; adz += FLT_EPSILON;
	// segment_dir X coord_axes
	if (std::abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return false;
	if (std::abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return false;
	if (std::abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return false;
	return true;
}
#pragma endregion

#pragma region AABB
bool Collision::AABBAABB(const AABB& a, const AABB& b)
{
	// x, y, z axis 
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (a.max[i] < b.min[i] || b.max[i] < a.min[i])
			return false;
	}
	return true;
}

bool Collision::AABBPlane(const AABB& b, const Plane& p)
{
	// center
	glm::vec3 c = (b.max + b.min) * 0.5f;
	// extents
	glm::vec3 e = b.max - c;
	// projection interval radius of b: L = b.c + t * p.n
	float r = e[0] * std::abs(p.normal[0]) + e[1] * std::abs(p.normal[1]) + e[2] * std::abs(p.normal[2]);
	// distance of box center from plane
	float s = glm::dot(p.normal, c) - p.d;
	// -r <= s <= r
	return std::abs(s) <= r;
}

#pragma endregion


glm::vec3 Collision::ClosestPtPointAABB(glm::vec3 p, AABB b)
{
	// clamp
	glm::vec3 ret;
	for (int i = 0; i < 3; i++) {
		float v = p[i];
		if (v < b.min[i]) v = b.min[i]; // v = max(v, b.min[i])
		if (v > b.max[i]) v = b.max[i]; // v = min(v, b.max[i])
		ret[i] = v;
	}
	return ret;
}

// Using extents here to get surface area
float Collision::AABB::GetSurfaceArea()
{
	float halfExtentX = (max.x - min.x) * 0.5f;
	float halfExtentY = (max.y - min.y) * 0.5f;
	float halfExtentZ = (max.z - min.z) * 0.5f;
	return abs(8.0f * ((halfExtentX * halfExtentY) + (halfExtentX * halfExtentZ) + (halfExtentY * halfExtentZ)));
}
