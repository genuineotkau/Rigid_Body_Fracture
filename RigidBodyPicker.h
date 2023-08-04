#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <stack>
#include "RigidBody.h"
#include "Camera.h"

class RigidBodyPicker
{
public:
	RigidBodyPicker(Camera* cam)
	{
		camera = cam;
	}

	void Traversal(BVHierarchy::Node* node, const Collision::Ray& ray, BVHierarchy::Node*& res, float& tmin,  glm::vec3& intersection) {
		if (node == nullptr)
			return;
		//res.emplace_back(root);
		float t;
		glm::vec3 q;
		bool isCollided = Collision::RayAABB(ray, node->BV_AABB, t, q);
		if (!isCollided) {
			return;
		}
		if (node->type == BVHierarchy::Node::Type::LEAF && !node->data->IsStatic()) {
			if (res == nullptr) {
				res = node;
				tmin = t;
				intersection = q;

			}
			else {
				if (t < 0.0) {
					assert(0);
				}
				if (t < tmin) {
					res = node;
					tmin = t;
					intersection = q;

				}
			}
		}

		Traversal(node->lChild, ray, res, t, intersection);
		Traversal(node->rChild, ray, res, t, intersection);
	}


	RigidBody* Click(Simulation* simulation, const glm::vec3& rayDir, glm::vec3& delta)
	{
		BVHierarchy::Node* node = simulation->GetTreeRoot();
		Collision::Ray ray(this->camera->Position, rayDir);
		float t;
		glm::vec3 q;
		BVHierarchy::Node* res = nullptr;
		Traversal(node, ray, res, t, q);
		if (res == nullptr) return nullptr;
		if (res->type != BVHierarchy::Node::Type::LEAF) {
			return nullptr;
		}
		delta = q;;
		return res->data;
	}
private:
	Camera* camera;
};