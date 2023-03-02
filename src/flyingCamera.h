#pragma once
#include "openglbase.h"

class FlyingCamera
{
public:

	FlyingCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 worldUp;

	glm::vec3 up;
	glm::vec3 right;

	float yaw;
	float pitch;

	float moveSpeed = 2.0f;


	// projection variable
	bool isPerspective = true;

	float fov = 45.0f;
	const float aspectRatio = (float)WIDTH / HEIGHT;
	float near = 0.1f;
	float far = 20.0f;

	int lastState = GLFW_RELEASE;

	// move position updated
	float lastX, lastY;
	bool firstMouse = true;

	//glm::mat4 view() { return createViewDirection(); }
	glm::mat4 view() { return createViewDirection(); }
	glm::mat4 projection() { return isPerspective ? createPerspective() : createOrthographic(); }


	void processKeyboardEvent(GLFWwindow* window, float deltaSeconds);
	void processMouseEvent(float xpos, float ypos);
	void processScrollEvent(float offset);

private:

	glm::mat4 createViewDirection();

	glm::mat4 createPerspective();
	glm::mat4 createOrthographic();

	void updateBaseVectors();

};