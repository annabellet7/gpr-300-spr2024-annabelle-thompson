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

#include <rainLib/Framebuffer.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();
void resetCamera(ew::Camera* camera, ew::CameraController* controller);
void processInput(GLFWwindow* window);

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;
ew::Camera camera;
ew::CameraController cameraController;

float hullThickness = 0.01;

struct PostProcessing
{
	bool bKuwaharaOn = true;
	bool aKuwaharaOn = true;
	int bRadius = 6;
	int aRadius = 3;
	bool edgeOn = true;
	float edge = 800.0f;
	bool gammaOn = true;
	float gamma = 2.2f;
	float threshold = 0.3f;
	const int SECTORS = 8;
	float sharpeness = 0.5f;
	float eccentricity = 1.0f;
	float mixValue = 0.5;
}postProcessing;

struct Lighting
{
	glm::vec3 lightDir = glm::vec3(1.0f, 0.0f, 1.0f);
	glm::vec3 lightColor = glm::vec3(0.9f, 0.9f, 0.9f);
}lighting;


struct Material {
	float Ka = 1.0;
	float Kd = 0.75;
	float Ks = 0.5;
	float Shininess = 128;
}material;

int main() {
	GLFWwindow* window = initWindow("Assignment 1", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader fruit = ew::Shader("assets/VertexShaders/fruit.vert", "assets/FragmentShaders/fruit.frag");
	ew::Shader stem = ew::Shader("assets/VertexShaders/stem.vert", "assets/FragmentShaders/stem.frag");
	ew::Shader monkey = ew::Shader("assets/VertexShaders/tangents.vert", "assets/FragmentShaders/monkey.frag");
	ew::Shader leaves = ew::Shader("assets/VertexShaders/tangents.vert", "assets/FragmentShaders/leaves.frag");
	ew::Shader hull = ew::Shader("assets/VertexShaders/hull.vert", "assets/FragmentShaders/hull.frag");
	ew::Shader gamma = ew::Shader("assets/VertexShaders/postprocessing.vert", "assets/FragmentShaders/gamma.frag");
	ew::Shader sobel = ew::Shader("assets/VertexShaders/postprocessing.vert", "assets/FragmentShaders/sobel.frag");
	ew::Shader overlay = ew::Shader("assets/VertexShaders/postprocessing.vert", "assets/FragmentShaders/overlay.frag");
	ew::Shader kuwahara = ew::Shader("assets/VertexShaders/postprocessing.vert", "assets/FragmentShaders/kuwahara.frag");

	fruit.use();
	fruit.setInt("uMainTex", 0);

	stem.use();
	stem.setInt("uMainTex", 1);

	monkey.use();
	monkey.setInt("uMainTex", 3);
	monkey.setInt("uMainNorms", 4);

	leaves.use();
	leaves.setInt("uMainTex", 5);
	leaves.setInt("uMainNorms", 6);

	gamma.use();
	gamma.setInt("uScreenTexture", 0);

	sobel.use();
	sobel.setInt("uScreenTexture", 0);

	kuwahara.use();
	kuwahara.setInt("uScreenTexture", 0);

	overlay.use();
	kuwahara.setInt("uScreenTexture", 0);
	overlay.setInt("uPaper", 2);

	ew::Model fruitModel = ew::Model("assets/Models/Fruit.obj");
	ew::Transform fruitTransform;
	ew::Model stemModel = ew::Model("assets/Models/Stem.obj");
	ew::Transform stemTransform;
	ew::Model monkeyModel = ew::Model("assets/Models/Suzanne.fbx");
	ew::Transform monkeyTransform;

	ew::MeshData planeMeshData = ew::createPlane(2.0f, 2.0f, 1.0f);
	ew::Mesh planeMesh = ew::Mesh(planeMeshData);
	ew::Transform planeTransform;

	GLuint fruitTexGamma = ew::loadTexture("assets/TextureColors/fruit_color.png", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, true);
	GLuint fruitTex = ew::loadTexture("assets/TextureColors/fruit_color.png", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, false);
	GLuint stemTexGamma = ew::loadTexture("assets/TextureColors/stem_color.png", GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR, false, true);
	GLuint stemTex = ew::loadTexture("assets/TextureColors/stem_color.png", GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR, false, false);
	GLuint stoneTexGamma = ew::loadTexture("assets/TextureColors/stone_color.jpg", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, true); //ambient cg
	GLuint stoneTex = ew::loadTexture("assets/TextureColors/stone_color.jpg", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, false); //ambient cg
	GLuint stoneNormals = ew::loadTexture("assets/TextureNormals/stone_normals.jpg", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, false); //ambient cg
	GLuint leafTexGamma = ew::loadTexture("assets/TextureColors/leaf_color.png", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, true); //ambient cg
	GLuint leafTex = ew::loadTexture("assets/TextureColors/leaf_color.png", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, false); //ambient cg
	GLuint leafNormals = ew::loadTexture("assets/TextureNormals/leaf_normals.png", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, false); //ambient cg
	GLuint paperTex = ew::loadTexture("assets/TextureColors/paper_color.jpg", GL_REPEAT, GL_LINEAR, GL_LINEAR, false, true); //texture labs

	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f;

	Framebuffer gammaBuffer;
	gammaBuffer.init(screenWidth, screenHeight);
	gammaBuffer.checkStatus();

	Framebuffer outlineBuffer;
	outlineBuffer.init(screenWidth, screenHeight);
	outlineBuffer.checkStatus();

	Framebuffer gKuwaharaBuffer;
	gKuwaharaBuffer.init(screenWidth, screenHeight);
	gKuwaharaBuffer.checkStatus();

	Framebuffer aKuwaharaBuffer;
	aKuwaharaBuffer.init(screenWidth, screenHeight);
	aKuwaharaBuffer.checkStatus();

	Framebuffer overlayBuffer;
	overlayBuffer.init(screenWidth, screenHeight);
	overlayBuffer.checkStatus();

	unsigned int dummyVAO;
	glCreateVertexArrays(1, &dummyVAO);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		processInput(window);

		//RENDER
		glBindFramebuffer(GL_FRAMEBUFFER, gammaBuffer.getFbo());
		glClearColor(lighting.lightColor.r, lighting.lightColor.g, lighting.lightColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		if (postProcessing.gammaOn)
		{
			glBindTextureUnit(0, fruitTexGamma);
			glBindTextureUnit(1, stemTexGamma);
			glBindTextureUnit(3, stoneTexGamma);
			glBindTextureUnit(5, leafTexGamma);
		}	
		else
		{
			glBindTextureUnit(0, fruitTex);
			glBindTextureUnit(1, stemTex);
			glBindTextureUnit(3, stoneTex);
			glBindTextureUnit(5, leafTex);
		}
		glBindTextureUnit(2, paperTex);
		glBindTextureUnit(4, stoneNormals);
		glBindTextureUnit(6, leafNormals);

		hull.use();
		glCullFace(GL_FRONT);
		hull.setFloat("uOutlineWidth", hullThickness);
		hull.setMat4("uModel", fruitTransform.modelMatrix()); 
		hull.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		fruitModel.draw();
		stemModel.draw();

		monkeyTransform.position = glm::vec3(3.0, 0.0, 0.0);
		hull.setMat4("uModel", monkeyTransform.modelMatrix());
		hull.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		monkeyModel.draw();

		planeTransform.position = glm::vec3(-3.0, 0.0, 0.0);
		planeTransform.rotation = glm::vec3(glm::radians(90.0), 0.0f, 0.0f);
		hull.setMat4("uModel", planeTransform.modelMatrix());
		hull.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		planeMesh.draw();

		//-----------------------------------------------------------------------------------

		fruit.use();
		glCullFace(GL_BACK);
		fruit.setVec3("uEyePos", camera.position);
		fruit.setFloat("uMaterial.Ka", material.Ka);
		fruit.setFloat("uMaterial.Kd", material.Kd);
		fruit.setFloat("uMaterial.Ks", material.Ks);
		fruit.setFloat("uMaterial.Shininess", material.Shininess);
		fruit.setVec3("uLightDir", lighting.lightDir);
		fruit.setVec3("uLightColor", lighting.lightColor);

		fruit.setMat4("uModel", fruitTransform.modelMatrix());
		fruit.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		fruitModel.draw();



		//-----------------------------------------------------------------------------------

		stem.use();

		stem.setVec3("uEyePos", camera.position);
		stem.setFloat("uMaterial.Ka", material.Ka);
		stem.setFloat("uMaterial.Kd", material.Kd);
		stem.setFloat("uMaterial.Ks", material.Ks);
		stem.setFloat("uMaterial.Shininess", material.Shininess);
		stem.setVec3("uLightDir", lighting.lightDir);
		stem.setVec3("uLightColor", lighting.lightColor);

		stem.setMat4("uModel", stemTransform.modelMatrix());
		stem.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		stemModel.draw();
		
		//-----------------------------------------------------------------------------------
		
		monkey.use();

		monkey.setVec3("uEyePos", camera.position);
		monkey.setFloat("uMaterial.Ka", material.Ka);
		monkey.setFloat("uMaterial.Kd", material.Kd);
		monkey.setFloat("uMaterial.Ks", material.Ks);
		monkey.setFloat("uMaterial.Shininess", material.Shininess);
		monkey.setVec3("uLightDir", lighting.lightDir);
		monkey.setVec3("uLightColor", lighting.lightColor);

		monkeyTransform.position = glm::vec3(3.0, 0.0, 0.0);
		monkey.setMat4("uModel", monkeyTransform.modelMatrix());
		monkey.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		monkeyModel.draw();

		//-----------------------------------------------------------------------------------

		leaves .use();

		leaves.setVec3("uEyePos", camera.position);
		leaves.setFloat("uMaterial.Ka", material.Ka);
		leaves.setFloat("uMaterial.Kd", material.Kd);
		leaves.setFloat("uMaterial.Ks", material.Ks);
		leaves.setFloat("uMaterial.Shininess", material.Shininess);
		leaves.setVec3("uLightDir", lighting.lightDir);
		leaves.setVec3("uLightColor", lighting.lightColor);

		planeTransform.position = glm::vec3(-3.0, 0.0, 0.0);
		planeTransform.rotation = glm::vec3(glm::radians(90.0), 0.0f, 0.0f);
		leaves.setMat4("uModel", planeTransform.modelMatrix());
		leaves.setMat4("uViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		planeMesh.draw();


		cameraController.move(window, &camera, deltaTime);
		glBindVertexArray(0);

		//-----------------------------------------------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, outlineBuffer.getFbo());
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		gamma.use();
		gamma.setFloat("uGamma", postProcessing.gamma);
		gamma.setInt("uGammaOn", postProcessing.gammaOn);

		glBindVertexArray(dummyVAO);
		glBindTexture(GL_TEXTURE_2D, gammaBuffer.getColorTexturebuffer());
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// -----------------------------------------------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, gKuwaharaBuffer.getFbo());
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		sobel.use();
		sobel.setFloat("uEdge", postProcessing.edge);
		sobel.setInt("uEdgeOn", postProcessing.edgeOn);
		sobel.setFloat("uThreshold", postProcessing.threshold);

		glBindVertexArray(dummyVAO);
		glBindTexture(GL_TEXTURE_2D, outlineBuffer.getColorTexturebuffer());
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//-----------------------------------------------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, overlayBuffer.getFbo());
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		kuwahara.use();
		kuwahara.setInt("uBKuwaharaOn", postProcessing.bKuwaharaOn);
		kuwahara.setInt("uBRadius", postProcessing.bRadius);
		kuwahara.setInt("uAKuwaharaOn", postProcessing.aKuwaharaOn);
		kuwahara.setInt("uARadius", postProcessing.aRadius);
		kuwahara.setInt("uSectors", postProcessing.SECTORS);
		kuwahara.setFloat("uSharpness", postProcessing.sharpeness);
		kuwahara.setFloat("uEccentricity", postProcessing.eccentricity);
		kuwahara.setFloat("uMix", postProcessing.mixValue);

		glBindVertexArray(dummyVAO);
		glBindTexture(GL_TEXTURE_2D, gKuwaharaBuffer.getColorTexturebuffer());
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//-----------------------------------------------------------------------------------
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		overlay.use();
		
		glBindVertexArray(dummyVAO);
		glBindTexture(GL_TEXTURE_2D, overlayBuffer.getColorTexturebuffer());
		glDrawArrays(GL_TRIANGLES, 0, 3);

		drawUI();
		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void drawUI() {
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
		ImGui::Checkbox("Basic Kuwahara ON/OFF", &postProcessing.bKuwaharaOn);
		ImGui::Checkbox("Anisotropic Kuwahara ON/OFF", &postProcessing.aKuwaharaOn);
		ImGui::Checkbox("Edge ON/OFF", &postProcessing.edgeOn);
		ImGui::Checkbox("Gamma ON/OFF", &postProcessing.gammaOn);
		ImGui::SliderInt("Basic Kuwahara radius", &postProcessing.bRadius, 0, 20);
		ImGui::SliderInt("Anisotropic Kuwahara radius", &postProcessing.aRadius, 0, 10);
		ImGui::SliderFloat("Sharpness", &postProcessing.sharpeness, 0.0f, 1.0f);
		ImGui::SliderFloat("Eccentricity", &postProcessing.eccentricity, 0.0f, 2.0f);
		ImGui::SliderFloat("Mix Kuwahara", &postProcessing.mixValue, 0.0f, 1.0f);
		ImGui::SliderFloat("Edge", &postProcessing.edge, 0.0f, 1000.0f);
		ImGui::SliderFloat("Threshold", &postProcessing.threshold, 0.0f, 1.0f);
		ImGui::SliderFloat("Hull Outlines", &hullThickness, 0.01, 0.05);
		ImGui::SliderFloat("Gamma", &postProcessing.gamma, 0.0f, 10.0f);
	}


	if (ImGui::Button("Reset Camera"))
	{
		resetCamera(&camera, &cameraController);
	}
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