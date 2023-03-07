#include "openglbase.h"
#include "shader.h"
#include "flyingCamera.h"
#include "texture.h"

void processInput(GLFWwindow* window, float deltaSeconds);

FlyingCamera camera;

bool focus = true;
int lastStateKeyF = GLFW_RELEASE;

int lastStateKeyF11 = GLFW_RELEASE;
bool fullScreen = true;

float lightMoveSpeed = 2.0f;

glm::vec3 worldRight(1.0f, 0.0f, 0.0f);
glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
glm::vec3 worldFront(0.0f, 0.0f, -1.0f);

const int pointLightCount = 4;
std::vector<glm::vec3> pointLightPositions = {
	glm::vec3(10.0f, 1.0f, 10.0f),
	glm::vec3(-10.0f, 1.0f, -10.0f),
	glm::vec3(10.0f, 1.0f, -10.0f),
	glm::vec3(-10.0f, 1.0f, 10.0f),
};
std::vector<glm::vec3> pointLightColors = {
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f)
};
std::vector<glm::vec3> pointLightSpecular = {
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f)
};
std::vector<float> pointLightConstants = {
	1.0f,
	1.0f,
	1.0f,
	1.0f
};
std::vector<float> pointLightLinears = {
	0.09f,
	0.09f,
	0.09f,
	0.09f
};
std::vector<float> pointLightQuadratics = {
	0.032f,
	0.032f,
	0.032f,
	0.032f
};


int lastStateKeyT = GLFW_RELEASE;
bool useSpotLight = true;


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
	int count = 0;
	GLFWmonitor** monitor = glfwGetMonitors(&count); 
	const GLFWvidmode* mode = glfwGetVideoMode(monitor[2]);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OPENGLBASE", monitor[2], NULL);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create the GLFW window");
	}

	WIDTH = mode->width;
	HEIGHT = mode->height;

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

		if(focus)
			camera.processMouseEvent(xpos, ypos);
	});
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		if(focus)
			camera.processScrollEvent(yoffset);
	});
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------


	// ---------------------------------------------------------------------------------
	// create vertexshader, fragmentshader, compile them and link to the shader program
	Shader globalShader("shader/global_vertex.glsl", "shader/global_fragment.glsl");
	Shader pointLightShader("shader/global_vertex.glsl", "shader/point_light_fragment.glsl");
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------
	// create texture
	Texture containerTexture("texture/container.png");
	Texture containerSpecularTexture("texture/container_specular.png");
	Texture emissionTexture("texture/container_emission.png");
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// ---------------------------------------------------------------------------------
	// 3d model of a 1x1x1 cube 
	std::vector<float> vertices = {
		// positions         // normals          // texture coords
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
		
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,

		 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f
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
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 8 * sizeof(float));

	// enable atribute and define vertex formats
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, false, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, false, 3 * sizeof(float));
	glEnableVertexArrayAttrib(VAO, 2);
	glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, false, 6 * sizeof(float));

	// associate vertex attributes and vertex buffer binding for a VAO
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribBinding(VAO, 2, 0);

	// bind the EBO to the VAO
	glVertexArrayElementBuffer(VAO, EBO);
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// ---------------------------------------------------------------------------------
	// cubes positions, rotations and colors
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

	std::vector<glm::vec3> cubeColors = {
		glm::vec3(.1f, 0.5f, .1f),
		glm::vec3(0.5f, 0.2f, 0.5f),
		glm::vec3(0.8f, 0.0f, 0.3f),
		glm::vec3(0.4f, 0.4f, 0.5f),
		glm::vec3(0.7f, 0.1f, 0.5f),
		glm::vec3(0.3f, 0.4f, 0.5f),
		glm::vec3(0.2f, 0.6f, 0.1f),
		glm::vec3(0.7f, 0.2f, 0.2f),
		glm::vec3(0.3f, 1.0f, 0.5f),
		glm::vec3(0.8f, 0.5f, 0.1f)
	};
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	// define the clear color (clear blue)
	glClearColor(.05f, 0.05f, .3f, 1.f);
	// enable depth comparaison (3D)
	glEnable(GL_DEPTH_TEST);



	// init IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");


	glm::vec3 directionalLightColor(1.0f);
	glm::vec3 directionalLightDirection(0.0f, -1.0f, 0.0f);
	glm::vec3 directionalLightSpecular(1.0f, 1.0f, 1.0f);


	glm::vec3 spotLightDirection(0.0f, 0.0f, -1.0f);
	glm::vec3 spotLightPosition(0.0f, 0.0f, 2.0f);
	float innerCutOff = 12.5f;
	float outerCutOff = 17.5f;
	glm::vec3 spotLightColor(1.0f);
	glm::vec3 spotLightLightDirection(0.0f, 0.0f, -1.0f);
	glm::vec3 spotLightLightSpecular(1.0f, 1.0f, 1.0f);
	float spotLightConstant = 1.0f;
	float spotLightLinear = 0.09f;
	float spotLightQuadratic = 0.032f;



	glm::vec3 lightColor(1.0f);


	// time between current frame and last frame
	float deltaSeconds = 0.0f;
	// time of last frame
	float lastFrameSeconds = 0.0f;
	
	// rotation animation values
	float angle = 0.05f;
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

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// bind the principal vertex array
		glBindVertexArray(VAO);

		// use the principal shader program
		globalShader.use();

		// bind texture and add a int uniform with the texture unit
		containerTexture.bind(0);
		containerSpecularTexture.bind(1);
		emissionTexture.bind(2);

		globalShader.uniform1i("material.diffuse", 0);
		globalShader.uniform1i("material.specular", 1);
		globalShader.uniform1i("material.emission", 2);

		globalShader.uniform1f("material.shininess", 32.0f);

		glm::mat4 view = camera.view();
		glm::mat4 projection = camera.projection();

		for (unsigned int i = 0; i < cubePositions.size(); i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, theta, cubeRotations[i]);
			theta += angle * deltaSeconds;

			globalShader.uniformMat4("model", model);
			globalShader.uniformMat4("view", view);
			globalShader.uniformMat4("projection", projection);


			glm::vec3 directionalDiffuseColor = directionalLightColor * glm::vec3(0.5f);
			glm::vec3 directionalAmbientColor = directionalDiffuseColor * glm::vec3(0.2f);

			globalShader.uniformVec3("directionalLight.direction", directionalLightDirection);
			globalShader.uniformVec3("directionalLight.ambient", directionalAmbientColor);
			globalShader.uniformVec3("directionalLight.diffuse", directionalDiffuseColor);
			globalShader.uniformVec3("directionalLight.specular", directionalLightSpecular);


			for (int i = 0; i < pointLightCount; i++)
			{

				glm::vec3 pointdiffuseColor = pointLightColors[i] * glm::vec3(0.5f);
				glm::vec3 pointAmbientColor = pointdiffuseColor * glm::vec3(0.2f);

				std::string pointLightName = "pointLight[" + std::to_string(i) + "]";
				globalShader.uniformVec3(pointLightName + ".position", pointLightPositions[i]);
				globalShader.uniformVec3(pointLightName + ".ambient", pointAmbientColor);
				globalShader.uniformVec3(pointLightName + ".diffuse", pointdiffuseColor);
				globalShader.uniformVec3(pointLightName + ".specular", pointLightSpecular[i]);
				globalShader.uniform1f(pointLightName + ".constant", pointLightConstants[i]);
				globalShader.uniform1f(pointLightName + ".linear", pointLightLinears[i]);
				globalShader.uniform1f(pointLightName + ".quadratic", pointLightQuadratics[i]);
			}

			glm::vec3 spotLightDiffuseColor = spotLightColor * glm::vec3(0.5f);
			glm::vec3 spotLightAmbientColor = spotLightDiffuseColor * glm::vec3(0.2f);

			globalShader.uniformVec3("spotLight.direction", spotLightDirection);
			globalShader.uniformVec3("spotLight.position", spotLightPosition);
			globalShader.uniform1f("spotLight.innerCutOff", glm::cos(glm::radians(innerCutOff)));
			globalShader.uniform1f("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));
			globalShader.uniformVec3("spotLight.ambient", spotLightAmbientColor);
			globalShader.uniformVec3("spotLight.diffuse", spotLightDiffuseColor);
			globalShader.uniformVec3("spotLight.specular", spotLightLightSpecular);
			globalShader.uniform1f("spotLight.constant", spotLightConstant);
			globalShader.uniform1f("spotLight.linear", spotLightLinear);
			globalShader.uniform1f("spotLight.quadratic", spotLightQuadratic);
			globalShader.uniform1i("useSpotLight", useSpotLight);


			globalShader.uniformVec3("viewPos", camera.position);


			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, GL_NONE);
		}

		for (int i = 0; i < pointLightCount; i++)
		{
			pointLightShader.use();

			glm::mat4 model(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));

			glm::mat4 mvp = projection * view * model;

			pointLightShader.uniformMat4("model", model);
			pointLightShader.uniformMat4("view", view);
			pointLightShader.uniformMat4("projection", projection);

			pointLightShader.uniformVec3("lightColor", lightColor);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, GL_NONE);
		}

		glm::mat4 model(1.0f);
		model = glm::translate(model, spotLightPosition);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 1.0f));

		glm::mat4 mvp = projection * view * model;

		pointLightShader.uniformMat4("model", model);
		pointLightShader.uniformMat4("view", view);
		pointLightShader.uniformMat4("projection", projection);

		pointLightShader.uniformVec3("lightColor", spotLightColor);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, GL_NONE);



		ImGui::Begin("imgui");
		ImGui::Text("camera");
		ImGui::MenuItem("Menu");
		ImGui::Checkbox("isPerpective", &camera.isPerspective);
		if (ImGui::TreeNode("Light Configuration"))
		{
			if (ImGui::TreeNode("Directional Light"))
			{
				ImGui::DragFloat3("direction", &directionalLightDirection[0]);
				ImGui::ColorEdit3("color", &directionalLightColor[0]);
				ImGui::DragFloat3("specular", &directionalLightSpecular[0]);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Point Lights"))
			{
				for (int i = 0; i < pointLightCount; i++)
				{
					if (ImGui::TreeNode("Point Light " + i))
					{
						ImGui::DragFloat3("position", &pointLightPositions[i][0]);
						ImGui::ColorEdit3("color", &pointLightColors[i][0]);
						ImGui::DragFloat3("specular", &pointLightSpecular[i][0]);
						ImGui::DragFloat("constant", &pointLightConstants[i]);
						ImGui::DragFloat("linear", &pointLightLinears[i]);
						ImGui::DragFloat("quadratic", &pointLightQuadratics[i]);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Spot Light"))
			{
				ImGui::DragFloat3("position", &spotLightPosition[0]);
				ImGui::DragFloat3("direction", &spotLightDirection[0]);
				ImGui::SliderFloat("innerCutOff", &innerCutOff, 0.0f, outerCutOff);
				ImGui::SliderFloat("outerCutOff", &outerCutOff, 0.0f, 90.0f);
				ImGui::ColorEdit3("color", &spotLightColor[0]);
				ImGui::DragFloat3("specular", &spotLightLightSpecular[0]);
				ImGui::DragFloat("constant", &spotLightConstant);
				ImGui::DragFloat("linear", &spotLightLinear);
				ImGui::DragFloat("quadratic", &spotLightQuadratic);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		



		//if (ImGui::TreeNode("Basic trees"))
		//{
		//	for (int i = 0; i < 5; i++)
		//	{
		//		// Use SetNextItemOpen() so set the default state of a node to be open. We could
		//		// also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
		//		if (i == 0)
		//			ImGui::SetNextItemOpen(true, ImGuiCond_Once);

		//		if (ImGui::TreeNode((void*)(intptr_t)i, "Child %d", i))
		//		{
		//			ImGui::Text("blah blah");
		//			ImGui::SameLine();
		//			if (ImGui::SmallButton("button")) {}
		//			ImGui::TreePop();
		//		}
		//	}
		//	ImGui::TreePop();
		//}

		ImGui::ShowDemoWindow();

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// compute events of the window
		glfwPollEvents();

		// swap the two buffers (Front -> Back | Back -> Front)
		glfwSwapBuffers(window);
	}
	// -----------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


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

	int stateKeyF = glfwGetKey(window, GLFW_KEY_F);
	if (stateKeyF == GLFW_PRESS && lastStateKeyF == GLFW_RELEASE)
	{
		if(focus)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		focus = !focus;

	}
	lastStateKeyF = stateKeyF;


	int stateKeyT = glfwGetKey(window, GLFW_KEY_T);
	if (stateKeyT == GLFW_PRESS && lastStateKeyT == GLFW_RELEASE)
	{
		useSpotLight = !useSpotLight;
	}
	lastStateKeyT = stateKeyT;

	int stateKeyF11 = glfwGetKey(window, GLFW_KEY_F11);
	if (stateKeyF11 == GLFW_PRESS && lastStateKeyF11 == GLFW_RELEASE)
	{

	}
	lastStateKeyF11 = stateKeyF11;

	if(focus)
		camera.processKeyboardEvent(window, deltaSeconds);
	else
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			pointLightPositions[0] += worldFront * lightMoveSpeed * deltaSeconds;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			pointLightPositions[0] -= worldFront * lightMoveSpeed * deltaSeconds;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			pointLightPositions[0] += worldRight * lightMoveSpeed * deltaSeconds;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			pointLightPositions[0] -= worldRight * lightMoveSpeed * deltaSeconds;

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			pointLightPositions[0] += worldUp * lightMoveSpeed * deltaSeconds;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			pointLightPositions[0] -= worldUp * lightMoveSpeed * deltaSeconds;
	}
}
