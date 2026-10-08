#pragma once
#include <vector>
#include "Entity.h"

class Scene {
public:
	std::vector<Entity> entities;
	Entity& AddEntity() {
		entities.emplace_back();
		return entities.back();
	}
};