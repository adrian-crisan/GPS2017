#include "stdafx.h"
#include "glad\glad.h"
#include "GLFW\glfw3.h"
#include "shader_s.h"
#include "camera.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "stb_image.h"

#include "model.h"
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//cube movement
float forwardCube = 0.0f;
float leftCube = 0.0f;
float rotateLeftCube = 0.0f;

int wireframe = 1;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GPS", glfwGetPrimaryMonitor(), NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("shaders/model.vert", "shaders/model.frag");
	Shader cubeShader("shaders/cube.vert", "shaders/cube.frag");
	Shader cubeShader2("shaders/cube2.vert", "shaders/cube2.frag");
	Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
	Shader lamp("shaders/lamp.vert", "shaders/lamp.frag");

	// load models
	// -----------
	Model droid("objects/droid/BattleDroidT.obj");
	Model ground("objects/ground/ground.obj");
	Model tree("objects/Tree 02/Tree.obj");
	Model nano("objects/nanosuit/nanosuit.obj");
	Model sphere("objects/sphere/webtrcc.obj");
	Model plane("objects/airplane/11805_airplane_v2_L2.obj");
	Model earth("objects/Earth/Earth.obj");
	Model castle("objects/hogwarts/great_hall.obj");

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	//cubes VAO
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces{
		"textures/skybox/darkskies_rt.jpg",
		"textures/skybox/darkskies_lf.jpg",
		"textures/skybox/darkskies_up.jpg",
		"textures/skybox/darkskies_dn.jpg",
		"textures/skybox/darkskies_bk.jpg",
		"textures/skybox/darkskies_ft.jpg"
	};

	unsigned int cubemapTexture = loadCubemap(faces);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	//cubes 
	unsigned int cubeDiffuse = loadTexture("textures/container2.png");
	unsigned int cubeSpecular = loadTexture("textures/container2_specular.png");
	unsigned int cubeDiffuse2 = loadTexture("textures/wood_box.jpg");
	unsigned int cubeDiffuse3 = loadTexture("textures/metal_box.jpg");
	
	cubeShader.use();
	cubeShader.setInt("material.diffuse", 0);
	cubeShader.setInt("material.specular", 1);

	cubeShader2.use();
	cubeShader2.setInt("material.diffuse", 0);
	cubeShader2.setVec3("material.specular", glm::vec3(0.4f, 0.5f, 0.4f));

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos(2*sin(glfwGetTime()), 1.5f, 2*cos(glfwGetTime()));

		ourShader.use();
		ourShader.setVec3("light.position", lightPos);
		ourShader.setVec3("viewPos", camera.Position);

		ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("light.constant", 1.0f);
		ourShader.setFloat("light.linear", 0.09f);
		ourShader.setFloat("light.quadratic", 0.032f);
		ourShader.setFloat("shininess", 32.0f);

		//directional light
		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
		ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		// don't forget to enable shader before setting uniforms
		ourShader.use();
		
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		glm::mat4 model;
	
		//droid																										//<---------------------------------
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.6f, 1.6f, 1.6f));	// had to scale the robot up because it was too small compared to the other objects from the scene
		model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		droid.Draw(ourShader);

		//tree
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-5.0f, -2.6f, -5.0f)); 
		ourShader.setMat4("model", model);
		tree.Draw(ourShader);

		//hogwarts
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(5.0f, -2.6f, -5.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		ourShader.setMat4("model", model);
		castle.Draw(ourShader);  

		//nanosuit
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-5.0f, -1.75f, 5.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		ourShader.setMat4("model", model);
		nano.Draw(ourShader);

		//nanosuit wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(5.0f, -1.75f, 5.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		ourShader.setMat4("model", model);
		nano.Draw(ourShader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//ground
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		ourShader.setMat4("model", model);
		ground.Draw(ourShader);

		//aircraft
		model = glm::mat4();
		model = glm::rotate(model, -1.5f * (float)(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(20.0f, 3.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.008f, 0.008f, 0.008f));
		ourShader.setMat4("model", model);
		plane.Draw(ourShader);

		//earth
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(40.0f, 5.75f, -30.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(-0.5f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		ourShader.setMat4("model", model);
		earth.Draw(ourShader);

		//earth wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-40.0f, 5.75f, -30.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(-0.5f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		ourShader.setMat4("model", model);
		earth.Draw(ourShader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//cubes
		cubeShader.use();
		cubeShader.setVec3("light.position", lightPos);
		cubeShader.setVec3("viewPos", camera.Position);
		cubeShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		cubeShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		cubeShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		cubeShader.setFloat("light.constant", 1.0f);
		cubeShader.setFloat("light.linear", 0.09f);
		cubeShader.setFloat("light.quadratic", 0.032f);

		cubeShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		cubeShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		cubeShader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
		cubeShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		// material properties
		cubeShader.setFloat("material.shininess", 32.0f);

		glm::mat4 projectionCube = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 viewCube = camera.GetViewMatrix();
		cubeShader.setMat4("projection", projectionCube);
		cubeShader.setMat4("view", viewCube);

		// world transformation
		glm::mat4 modelCube;
		modelCube = glm::translate(modelCube, glm::vec3(2.0f, -1.25f, 0.0f));
		cubeShader.setMat4("model", modelCube);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeDiffuse);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, cubeSpecular);

		glBindVertexArray(cubeVAO);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		//cube 2
		
		cubeShader2.use();
		cubeShader2.setVec3("light.position", lightPos);
		cubeShader2.setVec3("viewPos", camera.Position);
		cubeShader2.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		cubeShader2.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		cubeShader2.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		cubeShader2.setFloat("light.constant", 1.0f);
		cubeShader2.setFloat("light.linear", 0.09f);
		cubeShader2.setFloat("light.quadratic", 0.032f);

		cubeShader2.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		cubeShader2.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		cubeShader2.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
		cubeShader2.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// material properties
		cubeShader2.setFloat("material.shininess", 16.0f);

		glm::mat4 projectionCube2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 viewCube2 = camera.GetViewMatrix();
		cubeShader2.setMat4("projection", projectionCube2);
		cubeShader2.setMat4("view", viewCube2);

		// world transformation
		glm::mat4 modelCube2;
		modelCube2 = glm::translate(modelCube2, glm::vec3(2.0f, -1.25f, 2.0f));
		modelCube2 = glm::rotate(modelCube2, (float)(glfwGetTime()), glm::vec3(0.0f,1.0f,0.0f));
		cubeShader2.setMat4("model", modelCube2);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeDiffuse2);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//cube 3

		cubeShader2.use();
		cubeShader2.setVec3("light.position", lightPos);
		cubeShader2.setVec3("viewPos", camera.Position);
		cubeShader2.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		cubeShader2.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		cubeShader2.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		cubeShader2.setFloat("light.constant", 1.0f);
		cubeShader2.setFloat("light.linear", 0.09f);
		cubeShader2.setFloat("light.quadratic", 0.032f);

		// material properties
		cubeShader2.setFloat("material.shininess", 32.0f);

		glm::mat4 projectionCube3 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 viewCube3 = camera.GetViewMatrix();
		cubeShader2.setMat4("projection", projectionCube3);
		cubeShader2.setMat4("view", viewCube3);

		// world transformation
		glm::mat4 modelCube3;
		modelCube3 = glm::translate(modelCube3, glm::vec3(leftCube, -1.25f, forwardCube));

		//collision detection
		if (leftCube > 1.0f  &&
			leftCube < 3.0f &&
			forwardCube > 1.0f &&
			forwardCube < 3.0f) {
			leftCube = -2.0f;
			forwardCube = 0.0;
		}
		if (leftCube > 1.0f  &&
			leftCube < 3.0f &&
			forwardCube > -1.0f &&
			forwardCube < 1.0f) {
			leftCube = -2.0f;
			forwardCube = 0.0;
		}
		if (leftCube > -0.5f  &&
			leftCube < 0.5f &&
			forwardCube > -0.5f &&
			forwardCube < 0.5f) {
			leftCube = -2.0f;
			forwardCube = 0.0;
		}
		if (leftCube > -4.5f  &&
			leftCube < -3.5f &&
			forwardCube > -0.3f &&
			forwardCube < 0.3f) {
			leftCube = -2.0f;
			forwardCube = 0.0;
		}
		cubeShader2.setMat4("model", modelCube3);
		
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeDiffuse3);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//sphere
		lamp.use();
		lamp.setMat4("projection", projection);
		lamp.setMat4("view", view);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2 * sin(glfwGetTime()), 1.5f, 2 * cos(glfwGetTime())));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		lamp.setMat4("model", model);
		sphere.Draw(lamp);

		//draw skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, 2*deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, 2*deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, 2*deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, 2*deltaTime);

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		forwardCube += 0.05f;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		forwardCube -= 0.05f;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		leftCube -= 0.05f;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		leftCube += 0.05f;

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		rotateLeftCube -= 0.5f;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotateLeftCube += 0.5f;

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		wireframe = 0;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		wireframe = 1;
	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}