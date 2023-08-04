#ifndef SIMULATION_H
#define SIMULATION_H

#include "RigidBody.h"
#include "BVHierarchy.h"
#include <vector>

class Simulation
{
public:
	Simulation();

	std::vector<RigidBody*> objs;
	void SetLight(Shader* shader);
	void SetBoundary();
	void ResetSelectStatus();

	void UpdateAABB();
	void BuildBVH();
	void Render(const glm::mat4& projection, const glm::mat4& view);
	void RenderTree(BVHierarchy::Node** tree, const glm::mat4& projection, const glm::mat4& view);

	void UpdatePositions(GLfloat deltaTime);

	BVHierarchy::Node*& GetTreeRoot() { return *tree; }
	//void addShaderGroup(ShaderGroup shaderGroup);

	Model* treeModel;
	Shader* treeShader;

	bool isCheckMode = false;
	bool is_Demo7 = false;
	bool skipCheckStatic = false;

private:
	bool CollectColliders(RigidBody* rb, std::vector<RigidBody*>& collisions);
	void PerformCollision(RigidBody* rb1, RigidBody* rb2);
	void ClearTree();
	void FreeTree(BVHierarchy::Node* node);

	void Traversal(BVHierarchy::Node* root,RigidBody* rb, vector<BVHierarchy::Node*>& res);

	BVHierarchy::Node** tree;
	//bounding box, index 0 is min, index 1 is max
	float boundryX[2];
	float boundryY[2];
	float boundryZ[2];

};

#endif