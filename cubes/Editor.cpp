#include "Editor.h"
#include "imgui/imgui.h"

void Editor::Draw() {
	ImGui::Begin("Editor");
	ImGui::Text("cubes editor");
	ImGui::End();
}