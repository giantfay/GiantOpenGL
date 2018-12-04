#include "glad/glad.h"
#include "glfw3.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

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

GLuint load_texture(const char* imagePath);

class DirectionalLight
{
public:
	DirectionalLight(const std::string& name, const glm::vec3& color, const glm::vec3& dir, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
		:_name(name), _color(color), _direction(dir), _ambient(ambient), _diffuse(diffuse), _specular(specular)
	{

	}

	void SetShader(Shader& shader) const
	{
		shader.SetVec3(_name + ".direction", _direction);
		shader.SetVec3(_name + ".ambient", _ambient*_color);
		shader.SetVec3(_name + ".diffuse", _diffuse*_color);
		shader.SetVec3(_name + ".specular", _specular*_color);
	}
private:
	std::string _name;
	glm::vec3 _direction;
	glm::vec3 _ambient;
	glm::vec3 _diffuse;
	glm::vec3 _specular;
	glm::vec3 _color;
};

class PointLight
{
public:
	PointLight(const std::string& name,
		const glm::vec3& color,
		const glm::vec3& position,
		const glm::vec3& ambient,
		const glm::vec3& diffuse,
		const glm::vec3& specular,
		float constant,
		float linear,
		float quadratic)
		:_name(name),
		_color(color),
		_position(position),
		_ambient(ambient),
		_diffuse(diffuse),
		_specular(specular),
		_constant(constant),
		_linear(linear),
		_quadratic(quadratic)
	{

	}

	void SetShader(Shader& shader) const
	{
		shader.SetVec3(_name + ".position", _position);
		shader.SetVec3(_name + ".ambient", _ambient*_color);
		shader.SetVec3(_name + ".diffuse", _diffuse*_color);
		shader.SetVec3(_name + ".specular", _specular*_color);
		shader.SetFloat(_name + ".constant", _constant);
		shader.SetFloat(_name + ".linear", _linear);
		shader.SetFloat(_name + ".quadratic", _quadratic);
	}
private:
	std::string _name;
	glm::vec3 _position;
	glm::vec3 _ambient;
	glm::vec3 _diffuse;
	glm::vec3 _specular;
	float _constant;
	float _linear;
	float _quadratic;
	glm::vec3 _color;
};

class SpotLight
{
public:
	SpotLight(const std::string& name,
		const glm::vec3& color,
		const glm::vec3& position,
		const glm::vec3& direction,
		const glm::vec3& ambient,
		const glm::vec3& diffuse,
		const glm::vec3& specular,
		float constant,
		float linear,
		float quadratic,
		float cutOff,
		float outterCutOff)
		:_name(name),
		_color(color),
		_position(position),
		_direction(direction),
		_ambient(ambient),
		_diffuse(diffuse),
		_specular(specular),
		_constant(constant),
		_linear(linear),
		_quadratic(quadratic),
		_cutOff(cutOff),
		_outterCutOff(outterCutOff)
	{

	}

	void SetPos(const glm::vec3& pos)
	{
		_position = pos;
	}

	void SetDir(const glm::vec3& dir)
	{
		_direction = dir;
	}

	void SetShader(Shader& shader) const
	{
		shader.SetVec3(_name + ".position", _position);
		shader.SetVec3(_name + ".direction", _direction);
		shader.SetVec3(_name + ".ambient", _ambient*_color);
		shader.SetVec3(_name + ".diffuse", _diffuse*_color);
		shader.SetVec3(_name + ".specular", _specular*_color);
		shader.SetFloat(_name + ".constant", _constant);
		shader.SetFloat(_name + ".linear", _linear);
		shader.SetFloat(_name + ".quadratic", _quadratic);
		shader.SetFloat(_name + ".cutOff", _cutOff);
		shader.SetFloat(_name + ".outerCutOff", _outterCutOff);
	}
private:
	std::string _name;
	glm::vec3 _position;
	glm::vec3 _direction;
	glm::vec3 _ambient;
	glm::vec3 _diffuse;
	glm::vec3 _specular;
	float _constant;
	float _linear;
	float _quadratic;
	float _cutOff;
	float _outterCutOff;
	glm::vec3 _color;
};


int main()
{
	//init glfw
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create windwow
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "GinatOpenGL", nullptr, nullptr);
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
	Shader shader("../../Shaders/Colors/colors_vert.glsl", "../../Shaders/Colors/colors_frag.glsl");
	Shader lampShader("../../Shaders/Colors/lamp_vert.glsl", "../../Shaders/Colors/lamp_frag.glsl");
	//vertices
	GLfloat vertices[] = {
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

	glm::vec3 cubePositions[10] =
	{
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

	glm::vec3 pointLightsPos[4] = 
	{
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec3 pointLightColor[4] =
	{
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};

	//gen VAO, VBO
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//layout
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint lampVAO;
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	//Create and load texture
	GLuint tex1 = load_texture("../../Resources/Textures/container2.png");
	GLuint tex2 = load_texture("../../Resources/Textures/container2_specular.png");

	shader.Use();
	shader.SetInt("material.diffuseMap", 0);
	shader.SetInt("material.specularMap", 1);

	//DirectionalLight
	DirectionalLight dirLight("dirLight", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.2f, -1.0f, -0.3f),
										glm::vec3(0.05f, 0.05f, 0.05f), 
										glm::vec3(0.4f, 0.4f, 0.4f), 
										glm::vec3(0.5f, 0.5f, 0.5f));
	dirLight.SetShader(shader);
	//PointLight
	PointLight pointLights[4] = {
		PointLight("pointLights[0]", pointLightColor[0], pointLightsPos[0],
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f),
		PointLight("pointLights[1]", pointLightColor[1], pointLightsPos[1],
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f),
		PointLight("pointLights[2]", pointLightColor[2], pointLightsPos[2],
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f),
		PointLight("pointLights[3]", pointLightColor[3], pointLightsPos[3],
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f),
	};
	for (int i = 0; i < 4; ++i)
	{
		pointLights[i].SetShader(shader);
	}
	
	SpotLight spotLight("spotLight", glm::vec3(1.0f, 1.0f, 1.0f), camera.Position, camera.Front,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5)), glm::cos(glm::radians(15.0f)));
	spotLight.SetShader(shader);

	glEnable(GL_DEPTH_TEST);

	//render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 proj;
		proj = glm::perspective(glm::radians(camera.Fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		shader.SetMat4("projection", proj);

		glBindVertexArray(lampVAO);
		lampShader.Use();
		
		lampShader.SetMat4("view", view);
		lampShader.SetMat4("projection", proj);

		for (int i = 0; i < 4; ++i)
		{
			glm::mat4 lampModel;
			lampModel = glm::translate(lampModel, pointLightsPos[i]);
			lampModel = glm::scale(lampModel, glm::vec3(0.2f));
			lampShader.SetMat4("model", lampModel);
			lampShader.SetVec3("color", pointLightColor[i]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex2);

		glBindVertexArray(VAO);
		shader.Use();

		shader.SetVec3("viewPos", camera.Position);
		shader.SetMat4("view", view);
		shader.SetVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		shader.SetFloat("material.shininess", 32.0f);

		spotLight.SetPos(camera.Position);
		spotLight.SetDir(camera.Front);
		spotLight.SetShader(shader);

		for (int i = 0; i < 10; ++i)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(20.0f*i), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.SetMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		

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

GLuint load_texture(const char* imagePath)
{
	GLuint texID;
	glGenTextures(1, &texID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		switch (nrChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			format = GL_RED;
			break;
		}
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load image " << imagePath << std::endl;
	}
	stbi_image_free(data);

	return texID;
}