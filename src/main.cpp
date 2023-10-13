#include "openglbase.h"
#include "shader.h"
#include "flyingCamera.h"
#include "texture.h"
#include "model.h"
#include "sphere.h"
#include "light.h"
#include "cube.h"


void processInput(GLFWwindow* window, float deltaSeconds);

FlyingCamera camera;

bool focus = true;
int lastStateKeyF = GLFW_RELEASE;
glm::vec2 lastMousePosition;

int lastStateKeyF11 = GLFW_RELEASE;
bool fullScreen = true;

float lightMoveSpeed = 5.0f;

int lastStateKeyT = GLFW_RELEASE;
bool useSpotLight = true;


DirectionalLight directionalLight(glm::vec3(1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f));

std::vector<PointLight> pointLights = {
	PointLight(glm::vec3(1.0f), glm::vec3(10.0f, 1.0f, 10.0f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f),
	PointLight(glm::vec3(1.0f), glm::vec3(-10.0f, 1.0f, -10.0f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f),
	PointLight(glm::vec3(1.0f), glm::vec3(10.0f, 1.0f, -10.0f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f),
	PointLight(glm::vec3(1.0f), glm::vec3(-10.0f, 1.0f, 10.0f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f)
};

SpotLight spotLight(glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.0f), 12.5f, 17.5f, 1.0f, 0.09f, 0.032);


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
	const GLFWvidmode* mode = glfwGetVideoMode(monitor[0]);
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

	//Model testModel("model/backpack/backpack.obj");
	
	Material material("sphere material", 0, 0, 0, glm::vec3(1.0f), glm::vec3(1.0f), 256);

	Model testSphere(Sphere(), material);



	Model testCube(Cube(), Material("cube material", 0, 0, 0, glm::vec3(1.0f), glm::vec3(1.0f), 256));

	// ---------------------------------------------------------------------------------
	// create vertexshader, fragmentshader, compile them and link to the shader program
	Shader globalShader("shader/global_vertex.glsl", "shader/global_fragment.glsl");
	Shader pointLightShader("shader/global_vertex.glsl", "shader/point_light_fragment.glsl");
	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------

	// -----------------------------------------------------------------------------------
	// create texture
	Texture sphereTexture("texture/container.png", "");
	sphereTexture.bind(0);
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


		// use the principal shader program
		globalShader.use();

		

		glm::mat4 model(1.0f);
		glm::mat4 view = camera.view();
		glm::mat4 projection = camera.projection();

		for (unsigned int i = 0; i < 1; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f));
			//model = glm::rotate(model, theta, cubeRotations[i]);
			model = glm::scale(model, glm::vec3(1.0f));
			//theta += angle * deltaSeconds;

			globalShader.uniformMat4("model", model);
			globalShader.uniformMat4("view", view);
			globalShader.uniformMat4("projection", projection);


			glm::vec3 directionalDiffuseColor = directionalLight.color * glm::vec3(0.5f);
			glm::vec3 directionalAmbientColor = directionalDiffuseColor * glm::vec3(0.2f);

			globalShader.uniformVec3("directionalLight.direction", directionalLight.direction);
			globalShader.uniformVec3("directionalLight.ambient", directionalAmbientColor);
			globalShader.uniformVec3("directionalLight.diffuse", directionalDiffuseColor);
			globalShader.uniformVec3("directionalLight.specular", directionalLight.specular);


			for (int i = 0; i < pointLights.size(); i++)
			{

				glm::vec3 pointdiffuseColor = pointLights[i].color * glm::vec3(0.5f);
				glm::vec3 pointAmbientColor = pointdiffuseColor * glm::vec3(0.2f);

				std::string pointLightName = "pointLight[" + std::to_string(i) + "]";
				globalShader.uniformVec3(pointLightName + ".position", pointLights[i].position);
				globalShader.uniformVec3(pointLightName + ".ambient", pointAmbientColor);
				globalShader.uniformVec3(pointLightName + ".diffuse", pointdiffuseColor);
				globalShader.uniformVec3(pointLightName + ".specular", pointLights[i].specular);
				globalShader.uniform1f(pointLightName + ".constant", pointLights[i].constant);
				globalShader.uniform1f(pointLightName + ".linear", pointLights[i].linear);
				globalShader.uniform1f(pointLightName + ".quadratic", pointLights[i].quadratic);
			}

			glm::vec3 spotLightDiffuseColor = spotLight.color * glm::vec3(0.5f);
			glm::vec3 spotLightAmbientColor = spotLightDiffuseColor * glm::vec3(0.2f);

			globalShader.uniformVec3("spotLight.direction", spotLight.direction);
			globalShader.uniformVec3("spotLight.position", spotLight.position);
			globalShader.uniform1f("spotLight.innerCutOff", glm::cos(glm::radians(spotLight.inner)));
			globalShader.uniform1f("spotLight.outerCutOff", glm::cos(glm::radians(spotLight.outer)));
			globalShader.uniformVec3("spotLight.ambient", spotLightAmbientColor);
			globalShader.uniformVec3("spotLight.diffuse", spotLightDiffuseColor);
			globalShader.uniformVec3("spotLight.specular", spotLight.specular);
			globalShader.uniform1f("spotLight.constant", spotLight.constant);
			globalShader.uniform1f("spotLight.linear", spotLight.linear);
			globalShader.uniform1f("spotLight.quadratic", spotLight.quadratic);
			globalShader.uniform1i("useSpotLight", useSpotLight);


			globalShader.uniformVec3("viewPos", camera.position);


			testSphere.Draw(globalShader);

			//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, GL_NONE);
		}


		pointLightShader.use();
		for (int i = 0; i < pointLights.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLights[i].position);
			model = glm::scale(model, glm::vec3(0.2f));

			pointLightShader.uniformMat4("model", model);
			pointLightShader.uniformMat4("view", view);
			pointLightShader.uniformMat4("projection", projection);

			pointLightShader.uniformVec3("lightColor", pointLights[i].color);

			testCube.Draw(pointLightShader);
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, spotLight.position);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 1.0f));

		pointLightShader.uniformMat4("model", model);
		pointLightShader.uniformMat4("view", view);
		pointLightShader.uniformMat4("projection", projection);

		pointLightShader.uniformVec3("lightColor", spotLight.color);

		testCube.Draw(pointLightShader);

		


		ImGui::Begin("imgui");
		ImGui::Text("camera");
		ImGui::MenuItem("Menu");
		ImGui::Checkbox("isPerpective", &camera.isPerspective);
		if (ImGui::TreeNode("Light Configuration"))
		{
			if (ImGui::TreeNode("Directional Light"))
			{
				ImGui::DragFloat3("direction", &directionalLight.direction[0]);
				ImGui::ColorEdit3("color", &directionalLight.color[0]);
				ImGui::DragFloat3("specular", &directionalLight.specular[0]);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Point Lights"))
			{
				for (int i = 0; i < pointLights.size(); i++)
				{
					if (ImGui::TreeNode("Point Light " + i))
					{
						ImGui::DragFloat3("position", &pointLights[i].position[0]);
						ImGui::ColorEdit3("color", &pointLights[i].color[0]);
						ImGui::DragFloat3("specular", &pointLights[i].specular[0]);
						ImGui::DragFloat("constant", &pointLights[i].constant);
						ImGui::DragFloat("linear", &pointLights[i].linear);
						ImGui::DragFloat("quadratic", &pointLights[i].quadratic);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Spot Light"))
			{
				ImGui::DragFloat3("position", &spotLight.position[0]);
				ImGui::DragFloat3("direction", &spotLight.direction[0]);
				ImGui::SliderFloat("innerCutOff", &spotLight.inner, 0.0f, spotLight.outer);
				ImGui::SliderFloat("outerCutOff", &spotLight.outer, 0.0f, 90.0f);
				ImGui::ColorEdit3("color", &spotLight.color[0]);
				ImGui::DragFloat3("specular", &spotLight.specular[0]);
				ImGui::DragFloat("constant", &spotLight.constant);
				ImGui::DragFloat("linear", &spotLight.linear);
				ImGui::DragFloat("quadratic", &spotLight.quadratic);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}


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
		if (focus)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetCursorPos(window, camera.lastX, camera.lastY);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

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

	const glm::vec3 worldRight(1.0f, 0.0f, 0.0f);
	const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
	const glm::vec3 worldFront(0.0f, 0.0f, -1.0f);

	if(focus)
		camera.processKeyboardEvent(window, deltaSeconds);
	else
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			pointLights[0].position += worldFront * lightMoveSpeed * deltaSeconds;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			pointLights[0].position -= worldFront * lightMoveSpeed * deltaSeconds;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			pointLights[0].position += worldRight * lightMoveSpeed * deltaSeconds;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			pointLights[0].position -= worldRight * lightMoveSpeed * deltaSeconds;

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			pointLights[0].position += worldUp * lightMoveSpeed * deltaSeconds;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			pointLights[0].position -= worldUp * lightMoveSpeed * deltaSeconds;
	}
}
