#include "glad/glad.h"
#include "glfw3.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>

const int screenWidth = 800;
const int screenHeight = 600;

float mixFactor = 0.2f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = 400.0f;
float lastY = 300.0f;

bool firstMouse = true;

void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* windwo, double xoffset, double yoffset);

int main()
{
	//init glfw
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create windwow
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "ModelTest", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//init glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to init glad" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow* win, int width, int height)
	{
		glViewport(0, 0, width, height);
	});

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//lode shader file and compile
	Shader shader("../../Shaders/ModelTest/vert.glsl", "../../Shaders/ModelTest/frag.glsl");

	glEnable(GL_DEPTH_TEST);

	Model nanosuit("../../Resources/Objects/nanosuit/nanosuit.obj");

	//render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();

		glm::mat4 view = camera.GetViewMatrix();
		shader.SetMat4("view", view);
		glm::mat4 proj;
		proj = glm::perspective(glm::radians(camera.Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		shader.SetMat4("projection", proj);
		
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	
		shader.SetMat4("model", model);

		nanosuit.Draw(shader);


		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	float cameraSpeed = 2.5f*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (mixFactor <= 1.0f)
		{
			mixFactor += 0.001f;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (mixFactor >= 0.0f)
		{
			mixFactor -= 0.001f;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(xoffset, yoffset);
}