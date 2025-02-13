#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>
#include <ew/procGen.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI(int tex);
void resetCamera(ew::Camera* camera, ew::CameraController* controller);
void processInput(GLFWwindow* window);

//Global state
int screenWidth = 1080;
int screenHeight = 720;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
float nearPlane = 0.001f, farPlane = 50.0f;	
float prevFrameTime;
float deltaTime;
ew::Camera camera;
ew::CameraController cameraController;

struct Shadow
{
	float texel = 1.0f;
	float minBias = 0.005;
	float maxBias = 0.05;
}shadow;


struct PostProcessing
{
	bool blurOn = false;
	float blur = 300.0f;
	bool edgeOn = false;
	float edge = 300.0f;
	bool gammaOn = false;
	float gamma = 2.2f;
}postProcessing;

struct Lighting
{
	glm::vec3 lightDir = glm::vec3(0.5f, 1.0f, 0.0f);
	glm::vec3 lightColor = glm::vec3(0.95f, 0.875f, 0.8f);
}lighting;

struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

int main() {
	GLFWwindow* window = initWindow("Assignment 1", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Shader buffer = ew::Shader("assets/buffer.vert", "assets/buffer.frag");
	ew::Shader depth = ew::Shader("assets/depth.vert", "assets/depth.frag");
	ew::Shader debug = ew::Shader("assets/debug.vert", "assets/debug.frag");

	shader.use();
	shader.setInt("uMainTex", 0);
	shader.setInt("uMainNorms", 1);
	shader.setInt("uShadowMap", 2);

	buffer.use();
	buffer.setInt("uScreenTexture", 0);

	ew::Model monkeyModel = ew::Model("assets/Suzanne.fbx");
	ew::Transform monkeyTransform;

	ew::MeshData planeMeshData = ew::createPlane(10.0f, 10.0f, 1.0f);
	ew::Mesh planeMesh = ew::Mesh(planeMeshData);
	ew::Transform planeTransform;


	GLuint stoneTexGamma = ew::loadTexture("assets/stone_color.jpg", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, true);
	GLuint stoneTex = ew::loadTexture("assets/stone_color.jpg", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, false);
	GLuint stoneNormals = ew::loadTexture("assets/stone_normals.jpg");

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	unsigned int dummyVAO;
	glCreateVertexArrays(1, &dummyVAO);

	//----------Post Processing----------

	unsigned int framebuffer;
	unsigned int rbo;
	unsigned int textureColorbuffer;

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight); 
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------Shadow Mapping----------

	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		processInput(window);

		glClearColor(lighting.lightColor.r, lighting.lightColor.g, lighting.lightColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, nearPlane, farPlane);
		glm::mat4 lightView = glm::lookAt(lighting.lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		depth.use();
		depth.setMat4("uLightSpaceMatrix", lightSpaceMatrix);

		//Shadow Mapping
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			if (postProcessing.gammaOn)
				glBindTextureUnit(0, stoneTexGamma);
			else
				glBindTextureUnit(0, stoneTex);
			glBindTextureUnit(1, stoneNormals);
		{
			glCullFace(GL_FRONT);
			monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
			depth.setMat4("uModel", monkeyTransform.modelMatrix());
			monkeyModel.draw();

			planeTransform.position = glm::vec3(0.0f, -2.0f, 0.0f);
			depth.setMat4("uModel", planeTransform.modelMatrix());
			planeMesh.draw();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//reset viewport
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//RENDER
		//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glCullFace(GL_BACK);
		shader.use();

		if (postProcessing.gammaOn)
			glBindTextureUnit(0, stoneTexGamma);
		else
			glBindTextureUnit(0, stoneTex);
		glBindTextureUnit(1, stoneNormals);
		glBindTextureUnit(2, depthMap);

		shader.setVec3("uEyePos", camera.position);
		shader.setFloat("uMaterial.Ka", material.Ka);
		shader.setFloat("uMaterial.Kd", material.Kd);
		shader.setFloat("uMaterial.Ks", material.Ks);
		shader.setFloat("uMaterial.Shininess", material.Shininess);
		shader.setVec3("uLightDir", lighting.lightDir);
		shader.setVec3("uLightColor", lighting.lightColor);
		shader.setMat4("uLightSpaceMatrix", lightSpaceMatrix);
		shader.setFloat("uTexel", shadow.texel);
		shader.setFloat("uMinBias", shadow.minBias);
		shader.setFloat("uMaxBias", shadow.maxBias);

		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("uModel", monkeyTransform.modelMatrix());
		shader.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());

		monkeyModel.draw();

		planeTransform.position = glm::vec3(0.0f, -2.0f, 0.0f);
		shader.setMat4("uModel", planeTransform.modelMatrix());
		shader.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		planeMesh.draw();

		cameraController.move(window, &camera, deltaTime);

		/*glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		buffer.use();
		buffer.setFloat("uBlur", postProcessing.blur);
		buffer.setInt("uBlurOn", postProcessing.blurOn);
		buffer.setFloat("uEdge", postProcessing.edge);
		buffer.setInt("uEdgeOn", postProcessing.edgeOn);
		buffer.setFloat("uGamma", postProcessing.gamma);
		buffer.setInt("uGammaOn", postProcessing.gammaOn);

		glBindVertexArray(dummyVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	
		glDrawArrays(GL_TRIANGLES, 0, 3);*/

		drawUI(depthMap);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void drawUI(int tex) {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");

	if (ImGui::CollapsingHeader("Lighting")) 
	{
		ImGui::DragFloat3("Light Position", &lighting.lightDir.x, 0.1f);
		ImGui::ColorEdit3("Light Color", &lighting.lightColor.r, 0.1f);
	}
	

	if (ImGui::CollapsingHeader("Material")) 
	{
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}

	if (ImGui::CollapsingHeader("Post Processing"))
	{
		ImGui::Checkbox("Blur ON/OFF", &postProcessing.blurOn);
		ImGui::Checkbox("Edge ON/OFF", &postProcessing.edgeOn);
		ImGui::Checkbox("Gamma ON/OFF", &postProcessing.gammaOn);
		ImGui::SliderFloat("Blur", &postProcessing.blur, 0.0f, 300.0f);
		ImGui::SliderFloat("Edge", &postProcessing.edge, 0.0f, 300.0f);
		ImGui::SliderFloat("Gamma", &postProcessing.gamma, 0.0f, 10.0f);

	}

	if (ImGui::CollapsingHeader("Shadows"))
	{
		ImGui::SliderFloat("Min Bias", &shadow.minBias, 0.0f, 1.0f);
		ImGui::SliderFloat("Max Bias", &shadow.maxBias, 0.0f, 1.0f);
		ImGui::SliderFloat("PCF Filtering", &shadow.texel, 0.0f, 10.0f);
	}

	if (ImGui::Button("Reset Camera"))
	{
		resetCamera(&camera, &cameraController);
	}

	ImGui::End();

	ImGui::Begin("Shadow Map");
	//Using a Child allow to fill all the space of the window.
	ImGui::BeginChild("Shadow Map");
	//Stretch image to be window size
	ImVec2 windowSize = ImGui::GetWindowSize();
	//Invert 0-1 V to flip vertically for ImGui display
	//shadowMap is the texture2D handle
	ImGui::Image((ImTextureID)tex, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
	camera.aspectRatio = (float)width / height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller)
{
	camera->position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera->target = glm::vec3(0.0f);
	controller->yaw = 0.0f;
	controller->pitch = 0.0f;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}