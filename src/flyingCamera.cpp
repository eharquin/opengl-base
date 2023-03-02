#include "flyingCamera.h"

FlyingCamera::FlyingCamera(glm::vec3 position, float yaw, float pitch, glm::vec3 worldUp)
	: position(position), yaw(yaw), pitch(pitch), worldUp(worldUp)
{
	updateBaseVectors();
}

void FlyingCamera::processKeyboardEvent(GLFWwindow* window, float deltaSeconds)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += front * moveSpeed * deltaSeconds;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= front * moveSpeed * deltaSeconds;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += right * moveSpeed * deltaSeconds;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= right * moveSpeed * deltaSeconds;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += worldUp * moveSpeed * deltaSeconds;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		position -= worldUp * moveSpeed * deltaSeconds;

	int state = glfwGetKey(window, GLFW_KEY_P);
	if (state == GLFW_PRESS && lastState == GLFW_RELEASE)
		isPerspective = !isPerspective;
	lastState = state;
}

void FlyingCamera::processMouseEvent(float xpos, float ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	updateBaseVectors();
}

void FlyingCamera::processScrollEvent(float offset)
{
	fov -= (float)offset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void FlyingCamera::updateBaseVectors()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::cross(right, front);
}

glm::mat4 FlyingCamera::createViewDirection()
{
	glm::mat4 view(1.0f);
	view[0][0] = right.x;
	view[1][0] = right.y;
	view[2][0] = right.z;
	view[0][1] = up.x;
	view[1][1] = up.y;
	view[2][1] = up.z;
	view[0][2] = -front.x;
	view[1][2] = -front.y;
	view[2][2] = -front.z;
	view[3][0] = -glm::dot(right, position);
	view[3][1] = -glm::dot(up, position);
	view[3][2] = glm::dot(front, position);

	//   rx  ry  rz  -dot(r,p)
	//   ux  uy  uz  -dot(u,p)
	//   fx  fy  fz  -dot(f,p)
	//   0   0   0       1

	return view;
}

glm::mat4 FlyingCamera::createPerspective()
{
	glm::mat4 perspective(1.0f);
	perspective[0][0] = 1 / (aspectRatio * glm::tan(glm::radians(fov) / 2));
	perspective[1][1] = 1 / (tan(glm::radians(fov) / 2));
	perspective[2][2] = (near + far) / (near - far);
	perspective[3][2] = 2 * near * far / (near - far);
	perspective[2][3] = -1.0f;
	perspective[3][3] = 0.0f;

	//     1/(aspectRatio * tan(theta)        0               0              0 
	//                  0               1/(tan(theta)         0              0
	//                  0                     0         (-n-f)/(n-f)      2fn/(n-f)
	//                  0                     0               -1              0

	return perspective;
}

glm::mat4 FlyingCamera::createOrthographic()
{
	float ratioSizePerDepth = atan(glm::radians(fov / 2.0f)) * 2.0f;

	auto distance = glm::length(glm::vec3(0.0f, 0.0f, 0.0f) - position);

	float right = ratioSizePerDepth * distance * aspectRatio;
	float top = ratioSizePerDepth * distance;

	float n = 0.0f;
	float f = 2.0f * distance;

	glm::mat4 orthographic(1.0f);
	orthographic[0][0] = 1 / right;
	orthographic[1][1] = 1 / top;
	orthographic[2][2] = -2 / (n - f);
	orthographic[3][2] = -(n + f) / (n - f);

	// https://stackoverflow.com/questions/54987526/switching-from-perspective-to-orthogonal-keeping-the-same-view-size-of-model-and

	return orthographic;
}

