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

void Camera::setViewDirection(glm::vec3 cameraPosition, float yaw, float pitch, glm::vec3 worldUp)
{
	glm::vec3 direction;
	direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	direction.y = glm::sin(glm::radians(pitch));
	direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

	setViewDirection(cameraPosition, direction, worldUp);
}

void Camera::setOrthographicProjection(float left, float right, float bottom, float top, float near, float far)
{

	glm::mat4 scaleToCanonicalViewVolume(1.0f);
	scaleToCanonicalViewVolume[0][0] = 2 / (right - left);
	scaleToCanonicalViewVolume[1][1] = 2 / (top - bottom);
	scaleToCanonicalViewVolume[2][2] = 2 / (near - far);

	//   2/(r-l)   0       0      0
	//      0   2/(t-b)    0      0
	//      0      0     2/(n-f)  0
	//      0      0       0      1
	// openGL canonical view volume : -1 1 -1 1 1 -1

	glm::vec3 centerFromOrigin;
	centerFromOrigin.x = (right + left) / 2;
	centerFromOrigin.y = (top + bottom) / 2;
	centerFromOrigin.z = (near + far) / 2;

	//   (right + left) / 2
	//   (top + bottom) / 2
	//   (near + far) / 2

	glm::mat4 translateToOrigin(1.0f);
	translateToOrigin[3][0] = -centerFromOrigin.x;
	translateToOrigin[3][1] = -centerFromOrigin.y;
	translateToOrigin[3][2] = -centerFromOrigin.z;

	//   1  0  0  -(right + left) / 2
	//   0  1  0  -(top + bottom) / 2
	//   0  0  1   -(near + far) / 2
	//   0  0  0          1

	projection_ = scaleToCanonicalViewVolume * translateToOrigin;
	projection_[2][2] = -1 * projection_[2][2]; // switch the z-axis bc NDC is left-handed

	//   2/(r-l)     0        0        -(r+l)/(r-l)
	//      0     2/(t-b)     0        -(t+b)/(t-b)
	//      0        0     -2/(n-f)     -(n+f)/(n-f)
	//      0        0        0              1
}

void Camera::setPerspectiveProjection(float fov, float aspectRatio, float near, float far)
{
	// project x and y from the viewing frustrum to the near plane
	// x     (nx/-z)
	// y  => (ny/-z)
	// z       z
	// 1       1

	// ==>> IMPOSSIBLE BECAUSE ON A MAT4 * VECTOR WE CANNOT DIVIDE BY AN INPUT VALUE

	// SOLUTION : HOMOGENEOUS COORDINATES

	// x                          x/w
	// y  =after vertex shader=>  y/w
	// z                          z/w
	// w       

	// 1     10     2
	// 2  =  20  =  4
	// 3     30     6
	// 1     10     2

	// THIS DIVISION IS DIRECTLY APPLIED on the gl_position variable output from the vertex shader

	//    n   0   0   0
	//    0   n   0   0
	//    0   0   1   0
	//    0   0   1   0

	// x      x*n                          (xn)/-z
	// y  =>  y*n  =after vertex shader=>  (yn)/z
	// z       z                            z/-z ==> PROBLEM HERE : LOST DEPTH INFORMATION
	// 1       z                             


	// x      x*n                          (xn)/-z
	// y  =>  y*n  =after vertex shader=>  (yn)/-z
	// z       z²                           -z²/-z ==> z
	// 1       z                             

	//    n   0   0    0
	//    0   n   0    0
	//    0   0   m1   m2
	//    0   0   -1    0

	// -z² = m1*z + m2*1
	// QUADRATIC EQUATION WITH AT MOST TWO REAL SOLUTIONS : true for only two z values

	// we need to add two constraints: only true when z=near or z=far
	// ==> transformations will not change the z value for points on a near and far planes 
	// BUT all others z values will be wraped non-linearly.


	// this results in two equations :
	
	// m1 * n + m2 = -n²
	// m1 * f + m2 = -f²

	// m1 = - f - n
	// m2 = fn


	//    n   0      0      0
	//    0   n      0      0
	//    0   0    -f-n     fn
	//    0   0     -1      0


	// x          x*n                                 (xn)/-z
	// y  =>      y*n      =after vertex shader=>     (yn)/-z
	// z       -zf-zn+fn                             (-zf-zn+fn)/-z  =for n and f=> z
	// 1          -z                             


	// matrix from a frustrum view volume to a orthographic view volume
	glm::mat4 perspective(1.0f);
	perspective[0][0] = near;
	perspective[1][1] = near;
	perspective[2][2] = - far - near;
	perspective[3][2] = far * near;
	perspective[2][3] = -1.0f;
	perspective[3][3] = 0.0f;

	//    n   0    0     0
	//    0   n    0     0
	//    0   0    -f-n   fn
	//    0   0    -1     0


	// transformation from the view volume to the canonical view volume (see ortho function)
	//
	//   2/(r-l)     0        0        -(r+l)/(r-l)
	//      0     2/(t-b)     0        -(t+b)/(t-b)
	//      0        0      2/(n-f)     -(n+f)/(n-f)
	//      0        0        0              1


	// We assume frustrum is centered on the z-axis
	// r = -l
	// t = -b
	// then :
	// r + l = 0
	// r - l = 2r
	// t + b = 0
	// t - b = 2t
	
	//     1/r     0         0          0
	//      0     1/t        0          0
	//      0      0      2/(n-f)    -(n+f)/(n-f)
	//      0      0         0          1


	// use a vertical Field Of View : angle from the bottom to the top of the near plane
	// + the aspet ratio of the near plane : width/height

	// t = n*tan(fov/2)
	// r = n*a_r*tan(fov/2)
	 

	glm::mat4 projection(1.0f);
	projection[0][0] = 1 / (near * aspectRatio * glm::tan(fov / 2));
	projection[1][1] = 1 / (near * tan(fov / 2));
	projection[2][2] = 2 / (near - far);
	projection[3][2] = -(near + far) / (near - far);
	projection[3][3] = 1.0f;



	//projection_ = perspective * projection;
	projection_ = projection * perspective;
	projection_[2][2] = -1 * projection_[2][2]; // switch the z-axis bc NDC is left-handed

	//     1/(aspectRatio * tan(theta)        0               0              0 
	//                  0               1/(tan(theta)         0              0
	//                  0                     0         (-n-f)/(n-f)      2fn/(n-f)
	//                  0                     0               0              1
}
