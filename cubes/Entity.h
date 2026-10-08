#pragma once
#include <glm/glm.hpp>
#include "Transform.h"
#include "Mesh.h"

struct Entity {
	Transform transform;
	glm::vec3 color{1.0f};
	Mesh* mesh = nullptr;
};