#pragma once
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
	unsigned int cubeVAO;
	unsigned int lightVAO;
	unsigned int VBO;

	Mesh(const float* vertices, size_t vertexSize);
	void Draw();
};