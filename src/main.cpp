#include "openglbase.h"
#include "stb_image.h"
#include "shader.h"
#include "flyingCamera.h"

void processInput(GLFWwindow* window, float deltaSeconds);

FlyingCamera camera;

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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// define window callback
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
		// specify the viewport : "size of the screen"
		glViewport(0, 0, width, height);
	});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		camera.processMouseEvent(xpos, ypos);
	});
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.processScrollEvent(yoffset);
	});
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------


	// ---------------------------------------------------------------------------------
	// create vertexshader, fragmentshader, compile them and link to the shader program
	Shader shader("shader/vertex.glsl", "shader/fragment.glsl");
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------


	// ---------------------------------------------------------------------------------
	// 3d model of a 1x1x1 cube 
	std::vector<float> vertices = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	};

	std::vector<GLuint> indices = {
		0, 1 ,2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10 ,11,
		12, 13 ,14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	};
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------


	// -----------------------------------------------------------------------------------
	// create vertex buffer object and vertex arrays
	GLuint VBO, VAO, EBO;

	// create vertex buffer object to store raw vertices
	glCreateBuffers(1, &VBO);
	glNamedBufferData(VBO, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// create element buffer object to store indices
	glCreateBuffers(1, &EBO);
	glNamedBufferData(EBO, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// create vertex array to store the organized vertices
	glCreateVertexArrays(1, &VAO);

	// bind the VBO to the VAO
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(float));

	// enable atribute and define vertex formats
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, false, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, false, 3 * sizeof(float));

	// associate vertex attributes and vertex buffer binding for a VAO
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribBinding(VAO, 1, 0);

	// bind the EBO to the VAO
	glVertexArrayElementBuffer(VAO, EBO);
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// -----------------------------------------------------------------------------------
	// create texture
	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);

	// define texture wrapping (S,T like X,Y)
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set magnifying and minifying operations (when scaling up or downwards the image)
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// use stb lib to read the image
	stbi_set_flip_vertically_on_load(true);
	int widthTexture, heightTexture, numColCh;
	unsigned char* data = stbi_load("texture/magical.png", &widthTexture, &heightTexture, &numColCh, 0);
	if (!data)
		throw std::runtime_error("Failed to load texture");

	// submit image information to the texture
	glTextureStorage2D(texture, 1, GL_RGBA8, widthTexture, heightTexture);
	glTextureSubImage2D(texture, 0, 0, 0, widthTexture, heightTexture, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// generate mipmap (ref to minifying operation)
	glGenerateTextureMipmap(texture);

	stbi_image_free(data);
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// ---------------------------------------------------------------------------------
	// cubes psitions and rotations
	std::vector<glm::vec3> cubePositions = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	std::vector<glm::vec3> cubeRotations = {
		glm::vec3(0.1f,  0.0f,  0.0f),
		glm::vec3(1.0f,  0.5f, 1.0f),
		glm::vec3(-0.5f, 0.2f, 0.5f),
		glm::vec3(-0.8f, -0.0f, -0.3f),
		glm::vec3(0.4f, -0.4f, -0.5f),
		glm::vec3(-0.7f,  1.0f, -0.5f),
		glm::vec3(0.3f, -1.0f, -0.5f),
		glm::vec3(0.5f,  1.0f, -0.5f),
		glm::vec3(0.5f,  0.2f, -0.5f),
		glm::vec3(-0.3f,  1.0f, -0.5f)
	};
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// define the clear color (clear blue)
	glClearColor(.6f, .6f, .8f, 1.f);
	// enable depth comparaison (3D)
	glEnable(GL_DEPTH_TEST);


	// time between current frame and last frame
	float deltaSeconds = 0.0f;
	// time of last frame
	float lastFrameSeconds = 0.0f;
	
	// rotation animation values
	float angle = 0.005f;
	float theta = 0.0f;

	// -----------------------------------------------------------------------------------
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaSeconds = currentFrame - lastFrameSeconds;
		lastFrameSeconds = currentFrame;

		processInput(window, deltaSeconds);

		// clear the screen with the clear color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind the principal vertex array
		glBindVertexArray(VAO);

		// use the principal shader program
		shader.use();

		// bind texture and add a int uniform with the texture unit
		glBindTextureUnit(0, texture);
		shader.uniform1i("texture0", 0);

		glm::mat4 view = camera.view();
		glm::mat4 projection = camera.projection();

		for (unsigned int i = 0; i < cubePositions.size(); i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, theta, cubeRotations[i]);
			theta += angle * deltaSeconds;

			glm::mat4 mvp = projection * view * model;

			shader.uniformMatrix4("MVP", mvp);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, GL_NONE);
		}

		// compute events of the window
		glfwPollEvents();

		// swap the two buffers (Front -> Back | Back -> Front)
		glfwSwapBuffers(window);
	}
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------

	glDeleteTextures(1, &texture);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window, float deltaSeconds)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	camera.processKeyboardEvent(window, deltaSeconds);
}
