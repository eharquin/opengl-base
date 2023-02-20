#include "openglbase.h"

const int WIDTH = 800;
const int HEIGHT = 400;

int main()
{
	// init GLFW, add opengl version and use core profile (modern opengl)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window (define the viewport by default)
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OPENGLBASE", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create the GLFW window");
	}

	// introduce the window into the current context
	glfwMakeContextCurrent(window);

	// load glad 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Failed to use glad loader");

	// define callback to window resizing
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
			// specify the viewport : "size of the screen"
			std::cout << "test";
			glViewport(0, 0, width, height);
	});

	// define the clear color (VIOLET)
	glClearColor(.3, .1, .4, 1.);

	while (!glfwWindowShouldClose(window))
	{	
		// clear the screen with the clear color
		glClear(GL_COLOR_BUFFER_BIT);

		// compute events of the window
		glfwPollEvents();

		// swap the two buffers (Front -> Back | Back -> Front)
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}