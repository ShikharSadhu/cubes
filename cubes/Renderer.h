#pragma once
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"

class Renderer {
public:
	Renderer(Shader& shader, Shader& outlineShader);
	void Render(Scene& scene, Camera& camera, int selectedEntity, int frameBufferWidth, int frameBufferHeight);
private:
	Shader& shader;
	Shader& outlineShader;
};