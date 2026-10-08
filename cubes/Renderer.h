#pragma once
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"

class Renderer {
public:
	Renderer(Shader& shader);
	void Render(Scene& scene, Camera& camera, int frameBufferWidth, int frameBufferHeight);
private:
	Shader& shader;
};