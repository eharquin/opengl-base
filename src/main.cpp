#include "openglbase.h"

const int WIDTH = 800;
const int HEIGHT = 400;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OPENGLBASE", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create the GLFW window");
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
		throw std::runtime_error("Failed to use glad loader");


	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	});

	glClearColor(.3, .1, .4, 1.);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}