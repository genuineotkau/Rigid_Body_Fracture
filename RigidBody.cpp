#include "RigidBody.h"

RigidBody::RigidBody(Camera& camera, Shader* shader, Model* model, Mesh* mesh, glm::vec3 position, glm::vec3 acceleration, float mass, glm::vec3 torque, float rotationAngle, float angularVel, bool isStatic)
{
    this->camera = &camera;
    this->shader = shader;
    this->model = model;
    this->mesh = mesh;

    this->position = position;
    //this->force = force;
    this->acceleration = acceleration;
    this->velocity = glm::vec3(0.0f);
    this->mass = mass;

    this->rotationAngle = rotationAngle;
    this->angularVel = angularVel;
    this->torque = torque;
    this->isStatic = isStatic;
    this->isHide = false;
}

void RigidBody::UpdateVelocity(GLfloat deltaTime)
{
    //this->acceleration = this->force / this->mass;
    this->velocity += (this->acceleration + this->instantAcceleration) * deltaTime;
    if (this->instantAcceleration.x == 0.0f && this->instantAcceleration.y == 0.0f && this->instantAcceleration.z == 0.0f) {
    }
    else if (this->instantForceDuration > 0 ){
        this->instantForceDuration -= deltaTime;
        if (this->instantForceDuration < 0.0f) {
            this->instantForceDuration = 0;
            this->instantAcceleration = glm::vec3(0.0f);
        }
    }
}

void RigidBody::Move(GLfloat deltaTime)
{
    if (IsStatic() || IsStop()) return;
    this->position += this->velocity * deltaTime;
    //if (this->position.y < -1.0f) this->position.y = -1.0f;
    this->rotationAngle += this->angularVel * deltaTime;
}

void RigidBody::Render()
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, this->position);
    if (glm::length(torque) != 0)
        model = glm::rotate(model, rotationAngle, torque);

    shader->setMat4("model", model);

    //this->model->Draw(*(this->shader));
    if (this->mesh) {
        this->mesh->Draw(*(this->shader));
    }
    else {
        this->model->Draw(*(this->shader));
    }
}

