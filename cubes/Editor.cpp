#include "Editor.h"
#include "Scene.h"
#include "Mesh.h"
#include "imgui/imgui.h"

void Editor::Draw(Scene& scene, Mesh& cubeMesh) {
	ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 500.0f), ImGuiCond_FirstUseEver);
	
	ImGui::Begin("Editor");
	ImGui::Text("Scene");

	if (ImGui::Button("Add Cube")) {
		Entity& cube = scene.AddEntity();
		cube.name = "Cube" + std::to_string(scene.entities.size());
		cube.mesh = &cubeMesh;
	}

	ImGui::Separator();

	for (int i = 0; i < scene.entities.size(); i++) {
		Entity& entity = scene.entities[i];

		if (ImGui::Selectable(entity.name.c_str(), selectedEntity == i)) {
			selectedEntity = i;
		}
	}

	ImGui::End();

	DrawInspector(scene);
}

void Editor::DrawInspector(Scene& scene) {
	ImGui::SetNextWindowPos(ImVec2(340.0f, 20.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 500.0f), ImGuiCond_FirstUseEver);
	
	ImGui::Begin("Inspector");

	if (selectedEntity >= 0 && selectedEntity < scene.entities.size()) {
		Entity& entity = scene.entities[selectedEntity];
		ImGui::Text("%s", entity.name.c_str());

		ImGui::Separator();

		ImGui::Text("Position");
		ImGui::DragFloat3("##Position", &entity.transform.position.x, 0.1f);

		ImGui::Text("Rotation");
		ImGui::DragFloat3("##Rotation", &entity.transform.rotation.x, 1.0f);

		ImGui::Text("Scale");
		ImGui::DragFloat3("##Scale", &entity.transform.scale.x, 0.1f);
	}
	else {
		ImGui::Text("Nothing Selected");
	}

	ImGui::End();
}