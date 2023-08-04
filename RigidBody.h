#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"

#include "Collision.h"


class RigidBody
{
private:
	Camera* camera;
	Model* model;
	Mesh* mesh;
	bool collided;
	bool isSelect;
	bool isStatic;
	bool isStop = false;
	bool isHide;
public:
	RigidBody(Camera& camera, Shader* shader, Model* model, Mesh* mesh, glm::vec3 position, glm::vec3 velocity, float mass,
		glm::vec3 torque = glm::vec3(0, 0, 0), float rotationAngle = 0, float angularVel = 0, bool isStatic = false);
	void UpdateVelocity(GLfloat deltaTime);
	void Move(GLfloat deltaTime);
	void Render();
	void ClearAllMovement();

	glm::vec3 position;
	glm::vec3 force;
	glm::vec3 instantAcceleration;
	float instantForceDuration;
	glm::vec3 acceleration;
	glm::vec3 velocity;
	glm::vec3 torque;
	glm::vec3 color;

	float rotationAngle;
	float angularVel;
	float mass;
	float offsetY;

	// BV
	Collision::AABB aabb{ glm::vec3(0.f), glm::vec3(0.f) };
	std::vector<glm::vec3>& GetVerticePositions() { return (this->mesh) ? this->mesh->verticePositions : this->model->verticePositions; }

	bool IsSelect() { return isSelect; }
	void SetSelect(bool value) { isSelect = value; }
	
	bool IsStatic() { return isStatic; }
	void SetStop(bool value) { isStop = value; }
	bool IsStop() { return isStop; }

	void SetHide(bool value) { isHide = value; }
	bool IsHide() { return isHide; }

	void ApplyForce(glm::vec3 force, float duration) {
		this->instantAcceleration = force / mass;
		this->instantForceDuration = duration;
	}

	void ApplyG(glm::vec3 g) {
		this->acceleration = g;
	}

	bool IsPieces() { return mesh != nullptr; }

	Shader* shader;
};

#endif
