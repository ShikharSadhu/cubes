#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Transform.h"
#include "Mesh.h"

struct Entity {
	Transform transform;
	glm::vec3 color{1.0f};
	Mesh* mesh = nullptr;
	std::string name = "Entity";
};