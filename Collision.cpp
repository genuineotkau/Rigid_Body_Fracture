#include "Collision.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>

/**
    Referece material used: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/rtcd.pdf
**/

inline float TriArea2D(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return (x1 - x2) * (y2 - y3) - (x2 - x3) * (y1 - y2);
}

//  barycentric coordinates (u, v, w)
void Barycentric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& p, float& u, float& v, float& w)
{
	glm::vec3 m = glm::cross(b - a, c - a);
	// u, v nominators  one-over-denominator 
	float nu, nv, ood;
	float x = std::abs(m.x), y = std::abs(m.y), z = std::abs(m.z);

	if (x >= y && x >= z) {
		// Area of PBC in yz plane
		nu = TriArea2D(p.y, p.z, b.y, b.z, c.y, c.z);
		// Area of PCA in yz plane
		nv = TriArea2D(p.y, p.z, c.y, c.z, a.y, a.z);
		// 1/(2*area of ABC in yz plane)
		ood = 1.0f / m.x;
	}
	else if (y >= x && y >= z) {
		// project to the xz plane
		nu = TriArea2D(p.x, p.z, b.x, b.z, c.x, c.z);
		nv = TriArea2D(p.x, p.z, c.x, c.z, a.x, a.z);
		ood = 1.0f / -m.y;
	}
	else {
		// project to the xy plane
		nu = TriArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
		nv = TriArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
		ood = 1.0f / m.z;
	}
	u = nu * ood;
	v = nv * ood;
	w = 1.0f - u - v;
}

#pragma region Point
bool Collision::PointSphere(const glm::vec3& p, const Sphere& S)
{
	float d = glm::distance(p, S.position);
	float rr = (S.radius * S.radius);
	return ((d * d) <= rr);
}

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

bool Collision::PointTriangle(const glm::vec3& p, const Triangle& t)
{
	// face normal
	glm::vec3 normal = glm::normalize(glm::cross(t.v1, t.v2));
	Plane plane(normal, glm::dot(normal, t.v3));
	if (PointPlane(p, plane)) //if lie on the plane then test for barycentric
	{
		float u, v, w;
		Barycentric(t.v1, t.v2, t.v3, p, u, v, w);
		return v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f;
	}
	return false;
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

// ray r = o + td, return t and q
bool Collision::RaySphere(const Ray& r, const Sphere& s, float& t, glm::vec3& q)
{
	glm::vec3 m = r.start - s.position;
	float b = glm::dot(m, r.direction);
	float c = glm::dot(m, m) - s.radius * s.radius;
	// c > 0.0f: origin of ray outside of sphere
	// b > 0.0f: ray point away from sphere
	if (c > 0.0f && b > 0.0f) return false;
	float discr = b * b - c;
	if (discr < 0.0f) return false;
	t = - b - sqrt(discr);
	// t < 0.0f: ray is inside sphere, t = 0.0f;
	if (t < 0.0f) t = 0.0f;
	q = t * r.direction;
	return true;
}

// ray r = o + td, return tmin and q
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

// barycentric coordinates (u,v,w) of intersection point
bool Collision::RayTriangle(glm::vec3& p, glm::vec3& q, glm::vec3& a, glm::vec3& b, glm::vec3& c,
	float& u, float& v, float& w, float& t, glm::vec3& result)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;

	bool otherSide = false;
	glm::vec3 n = glm::cross(ac, ab);
	// denominator d, d <= 0: segment is parallel to or away from triangle
	float d = glm::dot(q, n);
	if (d <= 0.0f)
	{
		n = glm::cross(ab, ac);
		d = glm::dot(q, n);
		otherSide = true;

		if (d <= 0.0f)
			return false;
	}


	glm::vec3 pa = a - p;
	t = glm::dot(pa, n);
	if (t < 0.0f) return false;

	// barycentric coordinate components
	glm::vec3 e = glm::cross(q, pa);
	if (otherSide)
		e = glm::cross(pa, q);

	v = glm::dot(ac, e);
	if (v < 0.0f || v > d) return false;
	w = -glm::dot(ab, e);
	if (w < 0.0f || v + w > d) return false;

	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f - v - w;
	result = q * t;
	return true;
}
#pragma endregion

#pragma region Sphere
bool Collision::SphereSphere(const Sphere& a, const Sphere& b)
{
	return PointSphere(b.position, Sphere(a.position, a.radius + b.radius));
}

bool Collision::SphereAABB(const Sphere& a, const AABB& b)
{
	glm::vec3 closestPt = ClosestPtPointAABB(a.position, b);
	glm::vec3 v = closestPt - a.position;
	return glm::dot(v, v) <= a.radius * a.radius;
}

bool Collision::SpherePlane(const Sphere& s, const Plane& p)
{
	float dist = glm::dot(s.position, p.normal) - p.d;
	return std::abs(dist) <= s.radius;
}

#pragma endregion

#pragma region AABB
bool Collision::AABBAABB(const AABB& a, const AABB& b)
{
	// x, y, z axis 
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (i == 1) {
			//std::cout << "omgomg a: " << a.min[i] << ", " << a.max[i] << "; b: " << b.min[i] << ", " << b.max[i] << std::endl;
		}
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

bool Collision::AABBTriangle(const AABB& b, Triangle& t)
{
	float p0, p1, p2, r;
	glm::vec3 c = (b.min + b.max) * 0.5f;
	float e0 = (b.max.x - b.min.x) * 0.5f;
	float e1 = (b.max.y - b.min.y) * 0.5f;
	float e2 = (b.max.z - b.min.z) * 0.5f;
	// translate vector, AABB to origin
	t.v1 = t.v1 - c;
	t.v2 = t.v2 - c;
	t.v3 = t.v3 - c;

	// edge vectors
	glm::vec3 f0 = t.v2 - t.v1, f1 = t.v3 - t.v2, f2 = t.v1 - t.v3;

	p0 = t.v1.z * t.v2.y - t.v1.y * t.v2.z;
	p2 = t.v3.z * (t.v2.y - t.v1.y) - t.v3.z * (t.v2.z - t.v1.z);
	r = e1 * std::abs(f0.z) + e2 * std::abs(f0.y);
	// separated axis
	if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return false;

	if (std::max({ t.v1.x, t.v2.x, t.v3.x }) < -e0 || std::min({ t.v1.x, t.v2.x, t.v3.x }) > e0) return false;
	if (std::max({ t.v1.y, t.v2.y, t.v3.y }) < -e1 || std::min({ t.v1.y, t.v2.y, t.v3.y }) > e1) return false;
	if (std::max({ t.v1.z, t.v2.z, t.v3.z }) < -e2 || std::min({ t.v1.z, t.v2.z, t.v3.z }) > e2) return false;

	glm::vec3 normal = glm::cross(f0, f1);
	Plane p(normal, glm::dot(normal, t.v1));
	return AABBPlane(b, p);
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

float Collision::AABB::GetSurfaceArea()
{
	float halfExtentX = max.x - min.x * 0.5f;
	float halfExtentY = max.y - min.y * 0.5f;
	float halfExtentZ = max.z - min.z * 0.5f;
	return abs(8.0f * ((halfExtentX * halfExtentY) + (halfExtentX * halfExtentZ) + (halfExtentY * halfExtentZ)));
}

float Collision::Sphere::GetSurfaceArea()
{
	return 4.f * 3.14159f * radius * radius;
}
