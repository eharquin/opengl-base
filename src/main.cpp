#include "openglbase.h"

const int WIDTH = 800;
const int HEIGHT = 400;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

const char* vertexShaderSource = 
"#version 460 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vColor;\n"
"out vec2 vPos;\n"

"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"   vColor = aColor;\n"
"   vPos = aPos;\n"
"}\0";

const char* fragmentShaderSource = 
"#version 460 core\n"
"in vec3 vColor;\n"
"in vec2 vPos;\n"
"out vec4 FragColor;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"    vec3 denorm = (vColor * 255.0) / ((vPos.x + vPos.y) * 100);\n"
"    vec3 rounded = round(denorm);\n"
"    vec3 result = (rounded * ((vPos.x + vPos.y) * 100)) / 255.0;\n"
"    FragColor = vec4(result, 1.0);\n"
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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

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

	// init angle at 90° : first vertex will be in [0; 1]
	float angle = glm::pi<float>() / 2;
	int count = 0;
	float radius = 0.8f;

	while (angle <= (glm::pi<float>() * 2))
	{
		glm::vec2 v;
		v.x = radius * cos(angle);
		v.y = radius * sin(angle);

		vertices.push_back(v);
		angle += two_pi_over_three;
		count++;
	}
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------

	std::vector<float> colorVertices =
	{
		vertices[0].x, vertices[0].y, 1.0f, 0.0f, 0.0f,
		vertices[1].x, vertices[1].y, 0.0f, 1.0f, 0.0f,
		vertices[2].x, vertices[2].y, 0.0f, 0.0f, 1.0f
	};


	// -----------------------------------------------------------------------------------
	// create vertex buffer object and vertex arrays
	GLuint VBO, VAO;

	// create vertex buffer object to store raw vertices
	glCreateBuffers(1, &VBO);
	glNamedBufferData(VBO, colorVertices.size() * sizeof(float), colorVertices.data(), GL_DYNAMIC_DRAW);

	// create vertex array to store the organized vertices
	glCreateVertexArrays(1, &VAO);


	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(float));

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, false, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, false, 2 * sizeof(float));
	glVertexArrayAttribBinding(VAO, 1, 0);
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// define the clear color (clear blue)
	glClearColor(.6f, .6f, .8f, 1.f);


	// -----------------------------------------------------------------------------------
	// render loop
	float theta = .01f;
	while (!glfwWindowShouldClose(window))
	{	

		for (int i = 0; i < vertices.size(); i++)
		{
			float x = vertices[i].x * glm::cos(theta) - vertices[i].y * glm::sin(theta);
			float y = vertices[i].x * glm::sin(theta) + vertices[i].y * glm::cos(theta);

			vertices[i].x = x;
			vertices[i].y = y;
		}

		std::vector<float> colorVertices =
		{
			vertices[0].x, vertices[0].y, 1.0f, 0.0f, 0.0f,
			vertices[1].x, vertices[1].y, 0.0f, 1.0f, 0.0f,
			vertices[2].x, vertices[2].y, 0.0f, 0.0f, 1.0f
		};

		//std::vector<float> colorVertices =
		//{
		//	0.0f, 0.25f, 1.0f, 0.0f, 0.0f,
		//	0.25f, -0.25f, 0.0f, 1.0f, 0.0f,
		//	-0.25f, -0.25f, 0.0f, 0.0f, 1.0f
		//};

		glNamedBufferData(VBO, colorVertices.size() * sizeof(float), colorVertices.data(), GL_DYNAMIC_DRAW);

		// clear the screen with the clear color
		glClear(GL_COLOR_BUFFER_BIT);

		glm::vec4 color(1.0f);
		glUseProgram(shaderProgram);
		glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);

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