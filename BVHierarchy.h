#pragma once
#include "RigidBody.h"
#include "BoundingVolume.h"
#include "Collision.h"

namespace BVHierarchy
{
	enum class Tree
	{
		TOP_DOWN_MEDIAN_SPLIT,
		TOP_DOWN_K_EVEN_SPLIT,
		TOP_DOWN_MEDIAN_EXTENTS_SPLIT
	};

	struct Node
	{
		enum class Type
		{
			INTERNAL,
			LEAF
		};
		Collision::AABB BV_AABB{ {0.f, 0.f,0.f}, {0.f, 0.f, 0.f} };

		Node::Type type;
		RigidBody* data;
		int numObjects;
		int treeDepth = 0;
		glm::vec3 pos; //used only for bottom up for now

		Node* lChild, * rChild; // Child node pointers
	};

	Collision::AABB ComputeBoundingVolume(std::vector<RigidBody*>& objects, int startIndex, int numObjects);

	// Construct a top-down tree. Rearranges object[] array during construction
	void TopDownBVTree(Node** tree, std::vector<RigidBody*>& objects, int startIndex, int endIndex, int depth);
	float GetHeuristicCost(std::vector<RigidBody*>& objects, int startIndex, int split, int endIndex);
	int PartitionObjects(std::vector<RigidBody*>& objects, int startIndex, int endIndex);

	int FindIndexClosestToPoint(std::vector<RigidBody*>& objects, float point, int startIndex, int endIndex, char axis);
	int FindIndexWithExtents(std::vector<RigidBody*>& objects, float extent, int startIndex, int endIndex, char axis, bool renderSphere);
}

