#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "Mesh.h"
#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "Editor.h"

#include <glm/glm.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f);
Scene scene;

int frameBufferWidth = 800;
int frameBufferHeight = 600;


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

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

	//set up Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	Shader ourShader("3.3.shader.vs", "3.3.shader.fs");
	Renderer renderer(ourShader);
	Editor editor;

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

	Mesh cubeMesh(vertices, sizeof(vertices), indices, sizeof(indices));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	//render loop
	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		editor.Draw();
		
		renderer.Render(scene, camera, frameBufferWidth, frameBufferHeight);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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