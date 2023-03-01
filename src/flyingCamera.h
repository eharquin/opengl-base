#pragma once
#include "openglbase.h"

class FlyingCamera
{
public:

	FlyingCamera(glm::vec3 position, float yaw = -90.0f, float pitch = 0.0f, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 worldUp;

	glm::vec3 up;
	glm::vec3 right;

	float yaw;
	float pitch;

	float speed = 1.0f;

	glm::mat4 view() { return setViewDirection(); }


	void processKeyboardEvent(GLFWwindow* window, float deltaSeconds);
	void processMouseEvent(float xoffset, float yoffset);

private:

	glm::mat4 setViewDirection();
	void update();

};