#include "openglbase.h"

const int WIDTH = 800;
const int HEIGHT = 800;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

const char* vertexShaderSource = 
"#version 460 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\0";

const char* fragmentShaderSource = 
"#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(.4f, 0.3f, 0.7f, 1.0f);\n"
"}\n";

int main()
{
	// ---------------------------------------------------------------------------------
	// glfw, context and glad initialisation
	
	// init GLFW, add opengl version and use core profile (modern opengl)

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// create window (define the viewport by default)
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OPENGLBASE", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create the GLFW window");
	}

	// introduce the window into the current context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(true);

	// load glad 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Failed to use glad loader");

	// define callback to window resizing
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
		// specify the viewport : "size of the screen"
		glViewport(0, 0, width, height);
	});
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------


	// ---------------------------------------------------------------------------------
	// create vertexshader, fragmentshader, compile them and link to the shader program
	GLuint fragmentShader, vertexShader, shaderProgram;
	int success;
	char infoLog[512];

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_ERROR\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------


	// ---------------------------------------------------------------------------------
	// create 3 vertices placed equally around the unit circle : 360°/3 = 120° = (2pi)/3
	std::vector<glm::vec2> vertices;
	float constexpr two_pi_over_three = (2 * glm::pi<float>()) / 3;

	// init angle at 90° : first vertex will be at [0; 1]
	float angle = glm::pi<float>() / 2;
	int count = 0;

	while (angle <= (glm::pi<float>() * 2))
	{
		glm::vec2 v;
		v.x = cos(angle);
		v.y = sin(angle);

		vertices.push_back(v);
		angle += two_pi_over_three;
		count++;
	}
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// -----------------------------------------------------------------------------------
	// create vertex buffer object and vertex arrays
	GLuint VBO, VAO;

	// create vertex buffer object to store raw vertices
	glCreateBuffers(1, &VBO);
	glNamedBufferData(VBO, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);

	// create vertex array to store the organized vertices
	glCreateVertexArrays(1, &VAO);


	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 2 * sizeof(float));

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, false, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// define the clear color (clear blue)
	glClearColor(.6f, .6f, .8f, 1.f);


	// -----------------------------------------------------------------------------------
	// render loop
	while (!glfwWindowShouldClose(window))
	{	
		// clear the screen with the clear color
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, count);

		// compute events of the window
		glfwPollEvents();

		// swap the two buffers (Front -> Back | Back -> Front)
		glfwSwapBuffers(window);
	}
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------



	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}