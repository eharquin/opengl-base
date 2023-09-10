#include "openglbase.h"

const int width = 800;
const int height = 400;

int main()
{
	// init GLFW, add opengl version and use core profile (modern opengl)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window (define the viewport by default)
	GLFWwindow* window = glfwCreateWindow(width, height, "OPENGLBASE", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create the GLFW window" << std::endl;
		glfwTerminate();
	}

	// global draw loop
	while (!glfwWindowShouldClose(window))
	{
		// poll window events 
		glfwPollEvents();
		
		// .. //
	
	}


	glfwTerminate();

	return 0;
}