#include "BVHierarchy.h"

namespace BVHierarchy
{
	bool compareX(RigidBody* a, RigidBody* b) { return a->position.x < b->position.x; }
	bool compareY(RigidBody* a, RigidBody* b) { return a->position.y < b->position.y; }
	bool compareZ(RigidBody* a, RigidBody* b) { return a->position.z < b->position.z; }


	//AABB Method
	Collision::AABB ComputeBoundingVolume(std::vector<RigidBody*>& objects, int startIndex, int numObjects)
	{
		//assert(numObjects > 0);
		glm::vec3 Min = objects[startIndex]->aabb.min;
		glm::vec3 Max = objects[startIndex]->aabb.max;
		for (int i = startIndex + 1; i <= numObjects; ++i)
		{
			if (Min.x > objects[i]->aabb.min.x)
				Min.x = objects[i]->aabb.min.x;
			if (Max.x < objects[i]->aabb.max.x)
				Max.x = objects[i]->aabb.max.x;
			if (Min.y > objects[i]->aabb.min.y)
				Min.y = objects[i]->aabb.min.y;
			if (Max.y < objects[i]->aabb.max.y)
				Max.y = objects[i]->aabb.max.y;
			if (Min.z > objects[i]->aabb.min.z)
				Min.z = objects[i]->aabb.min.z;
			if (Max.z < objects[i]->aabb.max.z)
				Max.z = objects[i]->aabb.max.z;
		}
		return Collision::AABB(Min, Max);
	}

	void TopDownBVTree(Node** tree, std::vector<RigidBody*>& objects, int startIndex, int endIndex, int depth)
	{
		int numObjects = endIndex - startIndex + 1;
		const int MIN_OBJECTS_PER_LEAF = 1;
		Node* pNode = new Node;
		*tree = pNode;

		pNode->BV_AABB = ComputeBoundingVolume(objects, startIndex, endIndex);

		if (numObjects <= MIN_OBJECTS_PER_LEAF) {
			pNode->type = Node::Type::LEAF;
			pNode->numObjects = numObjects;
			pNode->data = objects[startIndex]; // Pointer to first object in leaf
			pNode->treeDepth = depth;
			pNode->lChild = nullptr;
			pNode->rChild = nullptr;
		}
		else {
			pNode->type = Node::Type::INTERNAL;
			pNode->treeDepth = depth;
			// Based on some partitioning strategy, arrange objects into
			// two partitions: object[0..k], and object[k+1..numObjects-1]
			int k = PartitionObjects(objects, startIndex, endIndex);
			// Recursively construct left and right subtree from subarrays and
			// point the left and right fields of the current node at the subtrees
			TopDownBVTree(&(pNode->lChild), objects, startIndex, k, depth + 1);
			TopDownBVTree(&(pNode->rChild), objects, k + 1, endIndex, depth + 1);
		}
	}

	//Chosen Heuristic: minimise total volume occupied by child nodes (volume proportional to SA)
	float GetHeuristicCost(std::vector<RigidBody*>& objects, // the array of objects
		int startIndex,
		int split, // the index to split into left & right
		int numObjects) // the total number of objects
	{
		if (numObjects == 1.f) //no child
		{
			Collision::AABB leftAABB = ComputeBoundingVolume(objects, startIndex, split);
			return leftAABB.GetSurfaceArea();
		}
			
		Collision::AABB leftAABB = ComputeBoundingVolume(objects, startIndex, split);
		float leftSurfaceArea = leftAABB.GetSurfaceArea();
		Collision::AABB rightAABB = ComputeBoundingVolume(objects, split + 1, numObjects);
		float rightSurfaceArea = rightAABB.GetSurfaceArea();
		return (split * leftSurfaceArea) +
			((numObjects - split - 1) * rightSurfaceArea);

	}

	int PartitionObjects(std::vector<RigidBody*>& objects, int startIndex, int endIndex)
	{
		int numObjects = endIndex - startIndex;
		float costX, costY, costZ;

		std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareX); //'X' PLANE
		costX = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2, numObjects);
		std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareY); //'Y' PLANE
		costY = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2, numObjects);
		std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareZ); //'Z' PLANE
		costZ = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2, numObjects);

		if (costX < costY && costX < costZ)
		{
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareX); //'X' PLANE
		}
		else if (costY < costX && costY < costZ)
		{
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareY); //'Y' PLANE
		}
		// else already sorted in z axis
		return startIndex + numObjects / 2; //MEDIAN SPLIT
	}

	int FindIndexClosestToPoint(std::vector<RigidBody*>& objects, float point, int startIndex, int endIndex, char axis)
	{
		float minDist = FLT_MAX;
		int closestIndex = startIndex;
		for (int i = startIndex; i <= endIndex; ++i)
		{
			float dist = 0;
			if (axis == 'x')
				dist = abs(objects[i]->position.x - point);
			else if (axis == 'y')
				dist = abs(objects[i]->position.y - point);
			else
				dist = abs(objects[i]->position.z - point);
			if (dist < minDist)
			{
				minDist = dist;
				closestIndex = i;
			}
		}
		return closestIndex;
	}

	int FindIndexWithExtents(std::vector<RigidBody*>& objects, float extent, int startIndex, int endIndex, char axis, bool renderSphere)
	{
		float minDist = FLT_MAX;
		int closestIndex = startIndex;
		for (int i = startIndex; i <= endIndex; ++i)
		{
			float dist = 0;

			if (axis == 'x')
			{
				dist = std::min(abs(objects[i]->position.x - objects[i]->aabb.min.x - extent), abs(objects[i]->position.x + objects[i]->aabb.max.x - extent));
			}
			else if (axis == 'y')
			{
				dist = std::min(abs(objects[i]->position.y - objects[i]->aabb.min.y - extent), abs(objects[i]->position.y + objects[i]->aabb.max.y - extent));
			}
			else
			{
				dist = std::min(abs(objects[i]->position.z - objects[i]->aabb.min.z - extent), abs(objects[i]->position.z + objects[i]->aabb.max.z - extent));
			}


			if (dist == 0.f)
			{
				return i; //return first object with that extent break out of loop
			}
		}
		return -1; //error the extents passed in do not belong to any of the objects
	}

}

