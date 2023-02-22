#pragma once
#include "openglbase.h"

class Camera
{
public:
	
	void setViewDirection(glm::vec3 cameraPosition, glm::vec3 cameraDirection, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));
	void setViewTarget(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

	void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
	void setPerspectiveProjection(float fov, float aspectRatio, float near, float far);


	glm::mat4 view() const { return view_; }
	glm::mat4 projection() const{ return projection_; }


private:
	glm::mat4 view_{ 1.0f };
	glm::mat4 projection_{ 1.0f };
};
