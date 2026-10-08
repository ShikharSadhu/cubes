#include "Renderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer(Shader& shader) : shader(shader) {

}

void Renderer::Render(Scene& scene, Camera& camera, int frameBufferWidth, int frameBufferHeight) {
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)frameBufferWidth / (float)frameBufferHeight, 0.1f, 100.0f);

	shader.use();
	int modelLoc = glGetUniformLocation(shader.ID, "model");
	int viewLoc = glGetUniformLocation(shader.ID, "view");
	int projLoc = glGetUniformLocation(shader.ID, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	for (Entity& entity : scene.entities) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), entity.transform.position);
		model = glm::rotate(model, glm::radians(entity.transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(entity.transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(entity.transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, entity.transform.scale);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		entity.mesh->Draw();
	}
}