#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), 5.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int frameBufferWidth = 800;
int frameBufferHeight = 600;


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Cubes", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	Shader ourShader("3.3.shader.vs", "3.3.shader.fs");

	float vertices[] = {
		//coords				//colors
		-0.5f, -0.5f,  0.5f,	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 1, 2,	2, 3, 0,
		1, 5, 6,	6, 2, 1,
		5, 4, 7,	7, 6, 5,
		4, 0, 3,	3, 7, 4,
		3, 2, 6,	6, 7, 3,
		4, 5, 1,	1, 0, 4
	};

	unsigned int VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glEnable(GL_DEPTH_TEST);

	//render loop
	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		//model matrix (with translation, scale and rotation)
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		
		//view matrix
		glm::mat4 view = camera.GetViewMatrix();

		//projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)frameBufferWidth / (float)frameBufferHeight, 0.1f, 100.0f);

		ourShader.use();

		int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(ourShader.ID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);

	frameBufferWidth = width;
	frameBufferHeight = height;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static float lastX = 400.0f;
	static float lastY = 300.0f;
	static bool firstMouse = true;

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera.Pan(xoffset, yoffset);
		else
			camera.Orbit(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.Zoom((float)yoffset);
}