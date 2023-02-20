#include "openglbase.h"

const int width = 800;
const int height = 400;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "OPENGLBASE", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create the GLFW window" << std::endl;
		glfwTerminate();
	}

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}


	glfwTerminate();

	return 0;
}