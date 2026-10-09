#include "Renderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer(Shader& shader, Shader& outlineShader) : shader(shader), outlineShader(outlineShader) {
}

void Renderer::Render(Scene& scene, Camera& camera, int selectedEntity, int frameBufferWidth, int frameBufferHeight) {
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)frameBufferWidth / (float)frameBufferHeight, 0.1f, 100.0f);

	shader.use();
	int modelLoc = glGetUniformLocation(shader.ID, "model");
	int viewLoc = glGetUniformLocation(shader.ID, "view");
	int projLoc = glGetUniformLocation(shader.ID, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glStencilMask(0x00);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);

	for (Entity& entity : scene.entities) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), entity.transform.position);
		model = glm::rotate(model, glm::radians(entity.transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(entity.transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(entity.transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, entity.transform.scale);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		entity.mesh->Draw();
	}

	if (selectedEntity >= 0 && selectedEntity < scene.entities.size()) {
		Entity& entity = scene.entities[selectedEntity];
		
		glDepthFunc(GL_LEQUAL);
		
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		shader.use();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), entity.transform.position);
        model = glm::rotate(model, glm::radians(entity.transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(entity.transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(entity.transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, entity.transform.scale);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        entity.mesh->Draw();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		glDisable(GL_DEPTH_TEST);

		outlineShader.use();

		int outlineModelLoc =
			glGetUniformLocation(outlineShader.ID, "model");

		int outlineViewLoc =
			glGetUniformLocation(outlineShader.ID, "view");

		int outlineProjLoc =
			glGetUniformLocation(outlineShader.ID, "projection");

		glUniformMatrix4fv(
			outlineViewLoc,
			1,
			GL_FALSE,
			glm::value_ptr(view)
		);

		glUniformMatrix4fv(
			outlineProjLoc,
			1,
			GL_FALSE,
			glm::value_ptr(projection)
		);

		glm::mat4 outlineModel =
			glm::translate(
				glm::mat4(1.0f),
				entity.transform.position
			);

		outlineModel = glm::rotate(
			outlineModel,
			glm::radians(entity.transform.rotation.x),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		outlineModel = glm::rotate(
			outlineModel,
			glm::radians(entity.transform.rotation.y),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		outlineModel = glm::rotate(
			outlineModel,
			glm::radians(entity.transform.rotation.z),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		outlineModel = glm::scale(
			outlineModel,
			entity.transform.scale * 1.05f
		);

		glUniformMatrix4fv(
			outlineModelLoc,
			1,
			GL_FALSE,
			glm::value_ptr(outlineModel)
		);

		entity.mesh->Draw();

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
}