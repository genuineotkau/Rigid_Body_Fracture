#include "Simulation.h"
#include "RigidBody.h"

#include <iostream>
#include <cmath>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/projection.hpp>

//#define DEBUG


Simulation::Simulation()
{
	tree = nullptr;
}
void Simulation::SetLight(Shader* shader)
{
	shader->use();
	shader->setVec3("light.position", glm::vec3(0.7f, 2.2f, 3.0f));
	shader->setVec3("light.ambient", 0.05f, 0.05f, 0.05f);
	shader->setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
	shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	shader->setFloat("light.constant", 1.0f);
	shader->setFloat("light.linear", 0.09f);
	shader->setFloat("light.quadratic", 0.032f);
}

void Simulation::SetBoundary()
{
	float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
	float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;
	for (int i = 0; i < this->objs.size(); i++) {
		if (!this->objs[i]->IsStatic()) continue;
		if (objs[i]->position.x < xmin) xmin = objs[i]->position.x;
		if (objs[i]->position.x > xmax) xmax = objs[i]->position.x;
		if (objs[i]->position.y < ymin) ymin = objs[i]->position.y;
		if (objs[i]->position.y > ymax) ymax = objs[i]->position.y;
		if (objs[i]->position.z < zmin) zmin = objs[i]->position.z;
		if (objs[i]->position.z > zmax) zmax = objs[i]->position.z;
	}

	// min and max value here should simply be floor, wall and ceiling, 
	// not hard-coded for the ability to scale
	boundryX[0] = xmin; boundryX[1] = xmax;
	boundryY[0] = ymin; boundryY[1] = ymax;
	if (boundryY[0] == boundryY[1]) boundryY[1] = FLT_MAX;	// if no ceiling, set to FLT_MAX
	boundryZ[0] = zmin; boundryZ[1] = zmax;

	#ifdef DEBUG
	std::cout << "Boundry X: " << boundryX[0] << " " << boundryX[1] << std::endl;
	std::cout << "Boundry Y: " << boundryY[0] << " " << boundryY[1] << std::endl;
	std::cout << "Boundry Z: " << boundryZ[0] << " " << boundryZ[1] << std::endl;
	#endif
}

void Simulation::ResetSelectStatus()
{
	for (int i = 0; i < this->objs.size(); i++) {
		this->objs[i]->SetSelect(false);
	}
}

//// Old method, Saved for reference
//void Simulation::UpdateAABB()
//{
//	for (int i = 0; i < this->objs.size(); i++) {
//		if (objs[i]->IsHide()) continue;
//		//if (!this->objs[i]->aabbCreated) {
//			this->objs[i]->aabb = BoundingVolume::createAABB(this->objs[i]->GetVerticePositions());
//		//}
//
//		this->objs[i]->offsetY = 0.0f - objs[i]->aabb.min.y;
//		glm::mat4 model(1.0f);
//		model = glm::translate(model, this->objs[i]->position);
//		if (glm::length(this->objs[i]->torque) != 0)
//			model = glm::rotate(model, this->objs[i]->rotationAngle, this->objs[i]->torque);
//
//		this->objs[i]->aabb.min = glm::vec3(model * glm::vec4(this->objs[i]->aabb.min, 1.0));
//		this->objs[i]->aabb.max = glm::vec3(model * glm::vec4(this->objs[i]->aabb.max, 1.0));
//
//		for (int j = 0; j < 3; ++j) {
//			if (this->objs[i]->aabb.min[j] > this->objs[i]->aabb.max[j]) {
//				float t = this->objs[i]->aabb.min[j];
//				this->objs[i]->aabb.min[j] = this->objs[i]->aabb.max[j];
//				this->objs[i]->aabb.max[j] = t;
//			}
//		}
//
//		//// If DEBUG is defined, print out the AABB
//		//#ifdef DEBUG
//		//	std::cout << "AABB min: " << this->objs[i]->aabb.min.x << " " << this->objs[i]->aabb.min.y << " " << this->objs[i]->aabb.min.z << std::endl;
//		//	std::cout << "AABB max: " << this->objs[i]->aabb.max.x << " " << this->objs[i]->aabb.max.y << " " << this->objs[i]->aabb.max.z << std::endl;
//		//#endif
//
//
//		//this->objs[i]->aabb.min = this->objs[i]->position + this->objs[i]->aabb.min;
//		//this->objs[i]->aabb.max = this->objs[i]->position + this->objs[i]->aabb.max;
//		//this->objs[i]->changedCollider = false;
//	}
//}

void Simulation::UpdateAABB()
{
	for (int i = 0; i < this->objs.size(); i++) {
		if (objs[i]->IsHide()) continue;

		// fixed aabb box
		this->objs[i]->aabb = BoundingVolume::createAABB(this->objs[i]->GetVerticePositions());
		this->objs[i]->offsetY = .0f - objs[i]->aabb.min.y;
		////print out the AABB value for debug
		//if (!objs[i]->IsStatic() && !objs[i]->IsStop()) {
		//	std::cout << "AABB min: " << this->objs[i]->aabb.min.x << " " << this->objs[i]->aabb.min.y << " " << this->objs[i]->aabb.min.z << std::endl;
		//	std::cout << "AABB max: " << this->objs[i]->aabb.max.x << " " << this->objs[i]->aabb.max.y << " " << this->objs[i]->aabb.max.z << std::endl;
		//	std::cout << "----------------------------------------------------" << std::endl;
		//}

		// Get a copy of the original vertices
		std::vector<glm::vec3> vertices = this->objs[i]->GetVerticePositions();

		glm::mat4 model(1.0f);
		model = glm::translate(model, this->objs[i]->position);
		if (glm::length(this->objs[i]->torque) != 0)
			model = glm::rotate(model, this->objs[i]->rotationAngle, this->objs[i]->torque);

		// Transform all vertices
		for (glm::vec3& vertex : vertices) {
			vertex = glm::vec3(model * glm::vec4(vertex, 1.0));
		}

		// Compute the AABB from the transformed vertices
		this->objs[i]->aabb = BoundingVolume::createAABB(vertices);
	}
}

void Simulation::BuildBVH()
{
	ClearTree();
	tree = new BVHierarchy::Node*;
	std::vector<RigidBody*> toBuildObjs;
	for (int i = 0; i < objs.size(); ++i) {
		if (!objs[i]->IsHide())
			toBuildObjs.push_back(objs[i]);
	}
	BVHierarchy::TopDownBVTree(tree, toBuildObjs, 0, toBuildObjs.size() - 1, 0);
	//BVHierarchy::Node* node = *tree;
}

void Simulation::Render(const glm::mat4& projection, const glm::mat4& view)
{
	for (int i = 0; i < this->objs.size(); i++) {
		if (objs[i]->IsHide()) continue;
		this->objs[i]->shader->use();
		this->objs[i]->shader->setMat4("projection", projection);
		this->objs[i]->shader->setMat4("view", view);
		this->objs[i]->shader->setBool("select", this->objs[i]->IsSelect());
		if (isCheckMode && this->objs[i]->color.r != 0.0f && this->objs[i]->color.g != 0.0f && this->objs[i]->color.b != 0.0f) {
			this->objs[i]->shader->setBool("isColorMode", true);
			this->objs[i]->shader->setVec3("color", this->objs[i]->color);
		}
		else {
			this->objs[i]->shader->setBool("isColorMode", false);
		}
		
		this->objs[i]->Render();
	}
}

void Simulation::RenderTree(BVHierarchy::Node** tree, const glm::mat4& projection, const glm::mat4& view)
{
	BVHierarchy::Node* node = *tree;
	if (node == nullptr)
		return;

	//if (node->treeDepth > renderDepth)
	//	return; //dont render the deeper nodes

	//Transform aabbTrans ;

	float scaleX = (node->BV_AABB.max.x - node->BV_AABB.min.x) * 0.5f;
	float scaleY = (node->BV_AABB.max.y - node->BV_AABB.min.y) * 0.5f;
	float scaleZ = (node->BV_AABB.max.z - node->BV_AABB.min.z) * 0.5f;

	glm::vec3 aabbScale{ scaleX, scaleY, scaleZ };
	glm::vec3 aabbCentre = (node->BV_AABB.max + node->BV_AABB.min) / 2.f;


	glm::mat4 model(1.0f);
	//model = glm::translate(model, glm::vec3(-0.5, -0.5, -0.5));
	model = glm::translate(model, aabbCentre);
	model = glm::scale(model, aabbScale);

	treeShader->use();
	treeShader->setMat4("projection", projection);
	treeShader->setMat4("view", view);
	treeShader->setMat4("model", model);
	//this->model->Draw(*(this->shader));
	//this->mesh->Draw(*(this->shader));

	//TreeDepth 0 Root Node
	glm::vec3 colour = glm::vec3(1.f, 0.f, 0.f); //Red
	if (node->treeDepth == 1)
		colour = glm::vec3(1.f, 0.5f, 0.f); //Orange
	else if (node->treeDepth == 2)
		colour = glm::vec3(1.f, 1.f, 0.f); //Yellow
	else if (node->treeDepth == 3)
		colour = glm::vec3(0.f, 1.f, 1.f); //Light Blue
	else if (node->treeDepth == 4)
		colour = glm::vec3(0.f, 0.f, 1.f); //Blue
	else if (node->treeDepth == 5)
		colour = glm::vec3(1.f, 0.f, 1.f); //Pink
	else if (node->treeDepth == 6)
		colour = glm::vec3(0.5f, 0.5f, 0.5f); //Grey

	//	colour = glm::vec3(0.f, 0.f, 1.f);
	//glUniform3f(glGetUniformLocation(programID, "renderColour"), colour.x, colour.y, colour.z);
	treeShader->setVec3("color", colour);
	//Draw
	if (node->type != BVHierarchy::Node::Type::INTERNAL)
		treeModel->DrawBoundingVolume();
	RenderTree(&node->lChild, projection, view);
	RenderTree(&node->rChild, projection, view);
}


void Simulation::UpdatePositions(GLfloat deltaTime)
{
	for (int i = 0; i < objs.size(); i++)
	{
		if (objs[i]->IsHide()) continue;
		RigidBody& obj = *objs[i];

		if (!skipCheckStatic && !objs[i]->IsStatic() && !objs[i]->IsStop()) {
			float smallVelocity = 0.06f;
			//float thresh = 0.1f;
			float attenuation = -.7f;
			float angleAttenuation = .7f;
			if (objs[i]->aabb.max.x > boundryX[1] && objs[i]->velocity.x > 0) {
				objs[i]->velocity.x *= attenuation;
				objs[i]->angularVel *= angleAttenuation;
			}
			else if (objs[i]->aabb.min.x < boundryX[0] && objs[i]->velocity.x < 0) {
				objs[i]->velocity.x *= attenuation;
				objs[i]->angularVel *= angleAttenuation;
			}

			if (objs[i]->aabb.max.y > boundryY[1] && objs[i]->velocity.y > 0) {
				objs[i]->velocity.y *= attenuation;
				objs[i]->angularVel *= angleAttenuation;
			}
			//else if (objs[i]->position.y < boundryY[0] - thresh && objs[i]->velocity.y < 0) {
			else if (objs[i]->aabb.min.y < boundryY[0] && objs[i]->velocity.y < 0) {
				objs[i]->velocity.y *= attenuation;
				objs[i]->angularVel *= angleAttenuation;
			}

			if (objs[i]->aabb.max.z > boundryZ[1] && objs[i]->velocity.z > 0) {
				objs[i]->velocity.z *= attenuation;
				objs[i]->angularVel *= angleAttenuation;
			}
			else if (objs[i]->aabb.min.z < boundryZ[0] && objs[i]->velocity.z < 0) {
				objs[i]->velocity.z *= attenuation;
				objs[i]->angularVel *= angleAttenuation;
			}

			if (abs(objs[i]->velocity.y) < smallVelocity && abs(objs[i]->aabb.min.y - boundryY[0]) < 0.1f) {
				objs[i]->rotationAngle = 0.0f;
				//objs[i]->angularVel = objs[i]->angularVel * 0.2f;
				//objs[i]->velocity = objs[i]->velocity * 0.2f;
				objs[i]->angularVel = 0.0f;
				objs[i]->velocity.x = 0.0f;
				objs[i]->velocity.y = 0.0f;
				objs[i]->velocity.z = 0.0f;
				
				//If obj is in the floor, or walls, move it out
				objs[i]->position.y = boundryY[0] + objs[i]->offsetY; //2* thresh;
				/*if(objs[i]->position.x < boundryX[0])
					objs[i]->position.x = boundryX[0];
				else if (objs[i]->position.x > boundryX[1])
					objs[i]->position.x = boundryX[1];
				if (objs[i]->position.z < boundryZ[0])
					objs[i]->position.z = boundryZ[0];
				else if (objs[i]->position.z > boundryZ[1])
					objs[i]->position.z = boundryZ[1];*/

				//if(glm::length(objs[i]->velocity) < 0.005)
					objs[i]->SetStop(true);
			}
			obj.Move(0.05);
			//UpdateAABB();
		}
		// if in DEBUG mode, print out the obj velocity
		#ifdef DEBUG
		if(!obj.IsHide() && !obj.IsStatic() && !obj.IsStop())
			std::cout << "obj " << i << " velocity: " << objs[i]->velocity.x << ", " << objs[i]->velocity.y << ", " << objs[i]->velocity.z << std::endl;
		#endif
		obj.UpdateVelocity(deltaTime);
		obj.Move(deltaTime);
		//UpdateAABB();
	}

	for (int i = 0; i < objs.size(); i++) {
		if (objs[i]->IsHide()) continue;
		std::vector<RigidBody*> collisions;
		if (objs[i]->IsStatic()) continue;
		CollectColliders(objs[i], collisions);

		for (int j = 0; j < collisions.size(); j++) {
			PerformCollision(objs[i], collisions[j]);
		}
	}
}

void Simulation::Traversal(BVHierarchy::Node* node, RigidBody* rb, std::vector<BVHierarchy::Node*>& res) {
	if (node == nullptr)
		return;
	//res.emplace_back(root);
	if (rb == node->data)
		return;

	//check for collision AABB
	bool isCollided = Collision::AABBAABB(rb->aabb, node->BV_AABB);
	if (!isCollided) {
		return;
	}

	if (node->type == BVHierarchy::Node::Type::LEAF) {
		res.push_back(node);
	}

	Traversal(node->lChild, rb, res);
	Traversal(node->rChild, rb, res);
}

bool Simulation::CollectColliders(RigidBody* rb, std::vector<RigidBody*>& collisions)
{
	BVHierarchy::Node* node = this->GetTreeRoot();
	std::vector<BVHierarchy::Node*> res;
	Traversal(node, rb, res);
	for (int i = 0; i < res.size(); i++) {
		collisions.push_back(res[i]->data);
	}
	return res.size() > 0;
}

void Simulation::ClearTree()
{
	if (this->tree != nullptr)
	{
		FreeTree(*tree);
		delete tree;
		tree = nullptr;
	}
}

void Simulation::FreeTree(BVHierarchy::Node* node)
{
	if (node == nullptr)
		return;
	//treeDepth = 0;
	FreeTree(node->lChild);
	FreeTree(node->rChild);
	delete node;
}

void Simulation::PerformCollision(RigidBody* rb1, RigidBody* rb2)
{
	if (rb1 == rb2) return;
	if (true)
	{
		//if (rb1->velocity.x == 0.0 && rb1->velocity.y == 0.0 && rb1->velocity.z == 0.0
		//	&& rb2->velocity.x == 0.0 && rb2->velocity.y == 0.0 && rb2->velocity.z == 0.0) {
		if (rb1->IsStop() && rb2->IsStop()) {
			return;
		}

		if (rb2->IsStatic()) { // if wall, skip
		}
		else {
			if (isCheckMode) {
				rb1->color = glm::vec3(rand() % 100 / (double)101, rand() % 100 / (double)101, rand() % 100 / (double)101);
				rb2->color = glm::vec3(rand() % 100 / (double)101, rand() % 100 / (double)101, rand() % 100 / (double)101);
				rb1->SetStop(true);
				rb2->SetStop(true);
				return;
			}

			if ((rb1->aabb.min.y < -0.5) && is_Demo7) {
				isCheckMode = true;
				rb1->SetStop(true);
				rb1->SetStop(true);
				return;
			}

			float attenuation_trans = 1.0f;
			float attenuation_rot = 1.0f;
			// glm::vec3 centersVector = (rb1->position - rb2->position);
			glm::vec3 rb1_pos = 0.5f * (rb1->aabb.min + rb1->aabb.max);
			glm::vec3 rb2_pos = 0.5f * (rb2->aabb.min + rb2->aabb.max);
			glm::vec3 centersVector = (rb1_pos - rb2_pos);
			centersVector = glm::normalize(centersVector);

			// projection of their current velocity on the centersVector direction
			glm::vec3 v1proj = glm::proj(rb1->velocity, centersVector);
			glm::vec3 v2proj = glm::proj(rb2->velocity, centersVector);

			float v1n = -1.0f * glm::length(v1proj);
			float v2n = glm::length(v2proj);

			float v1n_final = (v1n * (rb1->mass - rb2->mass) + 2 * (rb2->mass) * v2n) / (rb1->mass + rb2->mass);
			float v2n_final = (v2n * (rb2->mass - rb1->mass) + 2 * (rb1->mass) * v1n) / (rb1->mass + rb2->mass);
			glm::vec3 Ff1_dir = glm::vec3(0.0);
			if (rb1->velocity - v1proj != glm::vec3(0.0)) {
				Ff1_dir = glm::normalize(-(rb1->velocity - v1proj));
			}
			glm::vec3 Ff2_dir = glm::vec3(0.0);
			if (rb2->velocity - v2proj != glm::vec3(0.0)) {
				Ff2_dir = glm::normalize(-(rb2->velocity - v2proj));
			}

			rb1->velocity = attenuation_trans * ((rb1->velocity - v1proj) + (v1n_final * glm::normalize(centersVector)));
			rb2->velocity = attenuation_trans * ((rb2->velocity - v2proj) + (v2n_final * glm::normalize(centersVector)));

			float kfc_div_deltat = 0.1f;
			float Ff_norm = rb1->mass * (v1n_final - v1n) * kfc_div_deltat * 2;//equals to obj2.mass * (v2n_final - v2n) * kfc_div_deltat

			//calculate friction force
			glm::vec3 Ff1 = Ff1_dir * Ff_norm;
			glm::vec3 Ff2 = Ff2_dir * Ff_norm;

			glm::vec3 torque1 = glm::cross((-centersVector), Ff1);
			glm::vec3 torque2 = glm::cross((centersVector), Ff2);

			rb1->torque += torque1;
			rb2->torque += torque2;

			
			//float rb1_radius = glm::length(BoundingVolume::getExtents(rb1));
			//float rb2_radius = glm::length(BoundingVolume::getExtents(rb2));
			float alpha1 = glm::length(rb1->torque) / (2.0f/5.0f * rb1->mass * (0.93) * (0.93));//I = 2/5 * m * r^2
			float alpha2 = glm::length(rb2->torque) / (2.0f/5.0f * rb2->mass * (0.93) * (0.93));//I = 2/5 * m * r^2
			/*float alpha1 = glm::length(rb1->torque) / attenuation * (2.0f / 5.0f * rb2->mass * rb1_radius * rb1_radius);
			float alpha2 = glm::length(rb2->torque) / attenuation * (2.0f / 5.0f * rb2->mass * rb2_radius * rb2_radius);*/

			rb1->angularVel = attenuation_rot * alpha1;
			rb2->angularVel = attenuation_rot * alpha2;

			if (rb1->IsStop()) {
				rb1->ClearAllMovement();
			}

			if (rb2->IsStop()) {
				rb2->ClearAllMovement();
			}

			// Move() time step can be changed to anything that makes sense
			rb1->Move(0.1);
			rb2->Move(0.1);
			//UpdateAABB();
		}

	}
}

