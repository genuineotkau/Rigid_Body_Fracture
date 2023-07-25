#ifndef GAME_H
#define GAME_H

#include "RigidBody.h"
#include "BVHierarchy.h"
#include <vector>

class Game
{
public:
	Game();

	//std::vector<ShaderGroup> shaderGroups;
	//std::vector<RigidBody*> ballList;
	//std::vector<RigidBody*> wallList;
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

private:
	bool CollectColliders(RigidBody* rb, std::vector<RigidBody*>& collisions);
	void PerformCollision(RigidBody* rb1, RigidBody* rb2);
	void ClearTree();
	void FreeTree(BVHierarchy::Node* node);

	void Traversal(BVHierarchy::Node* root,RigidBody* rb, vector<BVHierarchy::Node*>& res);

	BVHierarchy::Node** tree;
	float boundryX[2];
	float boundryY[2];
	float boundryZ[2];

};

#endif