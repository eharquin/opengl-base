#include "camera.h"


//   rx  ry  rz  -px
//   ux  uy  uz  -py
//   dx  dy  dz  -pz
//   0   0   0    1
//
// r = cameraRight
// u = cameraUp
// d = cameraDirection
// p = cameraPosition
void Camera::setViewDirection(glm::vec3 cameraPosition, glm::vec3 cameraDirection, glm::vec3 worldUp)
{
	cameraDirection = glm::normalize(cameraDirection);

	// get the camera right vector pointing in the x-axis (camera space)
	glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
	
	// get the camera up vector (camera space)
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	// create a matrix that take a camer
	glm::mat4 cameraSpace(1.0f);
	cameraSpace[0][0] = cameraRight.x;
	cameraSpace[0][1] = cameraRight.y;
	cameraSpace[0][2] = cameraRight.z;

	cameraSpace[1][0] = cameraUp.x;
	cameraSpace[1][1] = cameraUp.y;
	cameraSpace[1][2] = cameraUp.z;

	cameraSpace[2][0] = cameraDirection.x;
	cameraSpace[2][1] = cameraDirection.y;
	cameraSpace[2][2] = cameraDirection.z;

	//   rx  ux  dx  0
	//   ry  uy  dy  0
	//   rz  uz  dz  0
	//   0   0   0   1

	// now we need to inverse the matrix to move all others objects to simulate camera rotation
	glm::mat4 inverseCameraSpace = glm::transpose(cameraSpace);

	//   rx  ry  rz  0
	//   ux  uy  uz  0
	//   dx  dy  dz  0
	//   0   0   0   1

	// we apply the inverse translation to all other objects to simulate camera displacement
	glm::mat4 inverseTranslation(1.0f);
	inverseTranslation[3][0] = -cameraPosition.x;
	inverseTranslation[3][1] = -cameraPosition.y;
	inverseTranslation[3][2] = -cameraPosition.z;

	//   1  0  0  -x
	//   0  1  0  -y
	//   0  0  1  -z
	//   0  0  0   1


	view_ = inverseCameraSpace * inverseTranslation;

	//   rx  ry  rz  -dot(r,p)
	//   ux  uy  uz  -dot(u,p)
	//   dx  dy  dz  -dot(d,p)
	//   0   0   0    1
}

void Camera::setViewTarget(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 worldUp)
{
	setViewDirection(cameraPosition, cameraPosition - cameraTarget, worldUp);
}
