#include "flyingCamera.h"

FlyingCamera::FlyingCamera(glm::vec3 position, float yaw, float pitch, glm::vec3 worldUp)
	: position(position), yaw(yaw), pitch(pitch), worldUp(worldUp)
{
	update();
}

void FlyingCamera::processKeyboardEvent(GLFWwindow* window, float deltaSeconds)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += front * speed * deltaSeconds;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= front * speed * deltaSeconds;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += right * speed * deltaSeconds;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= right * speed * deltaSeconds;
}

void FlyingCamera::processMouseEvent(float xoffset, float yoffset)
{
	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	update();
}

void FlyingCamera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(front);
	right = glm::normalize(glm::cross(worldUp, front));
	up = glm::cross(front, right);
}

glm::mat4 FlyingCamera::setViewDirection()
{
	glm::mat4 view(1.0f);
	view[0][0] = right.x;
	view[1][0] = right.y;
	view[2][0] = right.z;
	view[0][1] = up.x;
	view[1][1] = up.y;
	view[2][1] = up.z;
	view[0][2] = front.x;
	view[1][2] = front.y;
	view[2][2] = front.z;
	view[3][0] = -glm::dot(right, position);
	view[3][1] = -glm::dot(up, position);
	view[3][2] = -glm::dot(front, position);

	//   rx  ry  rz  -dot(r,p)
	//   ux  uy  uz  -dot(u,p)
	//   fx  fy  fz  -dot(f,p)
	//   0   0   0       1

	return view;
}

