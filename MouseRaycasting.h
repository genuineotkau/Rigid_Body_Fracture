#pragma once
//#define DEBUG
#include <glm/glm.hpp>
#include "Camera.h"
#include <iostream>

// reference material: course PDF and https://antongerdelan.net/opengl/raycasting.html

class MouseRaycasting
{
public:
	MouseRaycasting(Camera* cam)
	{
		camera = cam;
	}

	// saved for reference
	//glm::vec3 GenerateMouseRay(double x, double y, int scrWidth, int scrHeight)
	//{
	//	glm::vec3 ndc = GetNormalizedDeviceCoords(x, y, scrWidth, scrHeight);
	//	glm::vec4 clip = glm::vec4(ndc.x, ndc.y, -1.0, 1.0);
	//	glm::vec4 eye = GetEyeCoords(clip);
	//	glm::vec3 world = GetWorldCoords(eye);
	//	return world;
	//}

	// generate a ray from 2D screen space to 3D world space
	glm::vec3 GenerateMouseRay2(double x, double y, int scrWidth, int scrHeight, const glm::mat4& proj, const glm::mat4& view) {
		#ifdef DEBUG
		std::cout << "Mouse click screen pos x: " << x << " y: " << y << std::endl;
		#endif

		// mouseX and mouseY are in range [-1, 1]
		float mouseX = x/ (scrWidth * 0.5f) - 1.0f;
		float mouseY = y/ (scrHeight * 0.5f) - 1.0f;

		glm::mat4 invVP = glm::inverse(proj * view);
		glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
		glm::vec4 worldPos = invVP * screenPos;

		glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

		return dir;
	}

private:

	glm::vec3 GetNormalizedDeviceCoords(float x, float y, int scrWidth, int scrHeight)
	{
		float ndcX = (2.0f * x) / (double)scrWidth - 1.0f;
		float ndcY = 1.0f - (2.0f * y) / (double)scrHeight;	//y axis is flipped because it points down in screen space
		float ndcZ = 1.0;
		return glm::vec3(ndcX, ndcY, ndcZ);
	}

	glm::vec4 GetEyeCoords(glm::vec4 clip)
	{
		glm::vec4 eye = glm::inverse(camera->GetProjectionMatrix()) * clip;
		eye = glm::vec4(eye.x, eye.y, -1.0, 0.0);  // w set to 0 means a vector but not a point .
		return eye;

	}

	glm::vec3 GetWorldCoords(glm::vec4 eye)
	{
		glm::vec3 world = glm::inverse(camera->GetViewMatrix()) * eye;
		world = glm::normalize(world);
		return world;
	}

	Camera* camera;
};