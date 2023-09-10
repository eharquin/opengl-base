#include "openglbase.h"

const int WIDTH = 720;
const int HEIGHT = 720;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

const char* vertexShaderSource =
"#version 460 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aUV;\n"
"out vec2 vUV;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"   vUV = aUV;\n"

"}\0";

const char* fragmentShaderSource =
"#version 460 core\n"
"in vec2 vUV;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(vUV.x, vUV.y, vUV.x, 1.0f);\n"
"}\n";

struct Vertex
{
	glm::vec2 pos;
	glm::vec2 uv;
};

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
	glShaderSource(vertexShader, 1, &vertexShaderSource, GL_NONE);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, GL_NONE, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, GL_NONE);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, GL_NONE, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, GL_NONE, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_ERROR\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------

	std::vector<Vertex> vertices = {
		{ {-.75f,  .5f}, {0, 1} },
		{ { .75f,  .5f}, {1, 1} },
		{ { .75f, -.5f}, {1, 0} },
		{ {-.75f, -.5f}, {0, 0} }
	};

	std::vector<GLubyte> indices = {
		0, 1 ,2,
		0, 2, 3
	};

	// -----------------------------------------------------------------------------------
	// create vertex buffer object and vertex arrays
	GLuint VBO, VAO, EBO;

	// create vertex buffer object to store raw vertices
	glCreateBuffers(1, &VBO);
	glNamedBufferData(VBO, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// create element buffer object to store indices
	glCreateBuffers(1, &EBO);
	glNamedBufferData(EBO, indices.size() * sizeof(GLubyte), indices.data(), GL_STATIC_DRAW);

	// create vertex array to store the organized vertices
	glCreateVertexArrays(1, &VAO);

	// bind the VBO to the VAO
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 4 * sizeof(float));

	// enable atribute and define vertex format
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, false, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, false, sizeof(glm::vec2));

	// associate vertex attributes and vertex buffer binding for a VAO
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribBinding(VAO, 1, 0);

	// bind the EBO to the VAO
	glVertexArrayElementBuffer(VAO, EBO);
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
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, GL_NONE);

		// compute events of the window
		glfwPollEvents();

		// swap the two buffers (Front -> Back | Back -> Front)
		glfwSwapBuffers(window);
	}
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);


	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}