#pragma once
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	unsigned int indexCount;

	Mesh(const float* vertices, size_t vertexSize, const unsigned int* indices, size_t indexSize);
	void Draw();
};