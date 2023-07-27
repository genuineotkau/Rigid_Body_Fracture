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

	RigidBody* Pick(const vector<RigidBody*>& meshParts, const glm::vec3& ray)
	{
		// points on the meshPart
		RigidBody* selectedMeshPart = nullptr;
		for (RigidBody* meshPart : meshParts)
		{
			glm::vec3 pointLeftUpper = meshPart->position;
			float objWidth = .5f;
			float objHeight = .5f;
			glm::vec3 pointRightUpper = pointLeftUpper + glm::vec3(objWidth, 0, 0);
			glm::vec3 pointRightBottom = pointLeftUpper + glm::vec3(objWidth, -objHeight, 0);

			// normal vector of the meshPart
			glm::vec3 normal = glm::cross(pointRightBottom - pointLeftUpper, pointRightUpper - pointLeftUpper);
			// hitPoint = camera.Position + ray * t
			double t = glm::dot(pointLeftUpper - camera->Position, normal) / glm::dot(ray, normal);
			// intersection point
			glm::vec3 hitPoint = camera->Position + glm::vec3(ray.x * t, ray.y * t, ray.z * t);
			if (hitPoint.x >= pointLeftUpper.x && hitPoint.x <= pointRightUpper.x && hitPoint.y <= pointLeftUpper.y && hitPoint.y >= pointRightBottom.y)
			{
				// TODO: find the meshPart closet to the camera
				if (selectedMeshPart == nullptr)
				{
					selectedMeshPart = meshPart;
					std::cout << "RididBody " << selectedMeshPart->mass << " Selected\n";
					break;
				}
			}
		}
		return selectedMeshPart;
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
		//cout << "omgomg click! " << endl;
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
		delta = q;
		return res->data;


		//auto bvh_nodes = simulation->GetLinearBVHNodes();
		//std::stack<int> nodesToVisit;
		//int cur_node_index = 0;
		//while (true)
		//{
		//	auto node = bvh_nodes[cur_node_index];
		//	//if (node.boundingbox.isIntersect(ray, XMMatrixIdentity()))
		//	float tmin; 
		//	glm::vec3 q;
		//	Collision::Ray ray(this->camera->Position, rayDir);
		//	if (Collision::RayAABB(ray, node->BV_AABB, tmin, q))
		//	{
		//		if (node->numObjects > 0)
		//		{
		//			for (int i = 0; i < node->numObjects; i++)
		//			{
		//				float t = -1.0f;
		//				Primitive* p = objectManager.getBVHManager()->getPrimitive(node.primitivesOffset + i);
		//				IntersectInfo it;
		//				if (p->is_intersect(ray, t, it) && (min_t < 0.0f || t < min_t))
		//				{
		//					min_t = t;
		//					info = it;
		//				}
		//			}
		//			if (nodesToVisit.empty())
		//				break;
		//			cur_node_index = nodesToVisit.top();
		//			nodesToVisit.pop();
		//		}
		//		else
		//		{
		//			if (ray.dirIsNeg(node.axis))
		//			{
		//				nodesToVisit.push(cur_node_index + 1);
		//				cur_node_index = node.secondChildOfset;

		//			}
		//			else
		//			{
		//				nodesToVisit.push(node.secondChildOfset);
		//				cur_node_index++;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		if (nodesToVisit.empty())
		//			break;
		//		cur_node_index = nodesToVisit.top();
		//		nodesToVisit.pop();
		//	}
		//}
	}
private:
	Camera* camera;
};