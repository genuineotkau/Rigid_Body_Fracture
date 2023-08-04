#pragma once
#include "RigidBody.h"
#include "BoundingVolume.h"
#include "Collision.h"

/**
	Referece material used: https://ceng2.ktu.edu.tr/~cakir/files/grafikler/rtcd.pdf
**/

namespace BVHierarchy
{
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
}

