#include "Editor.h"
#include "Scene.h"
#include "Mesh.h"
#include "imgui/imgui.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <algorithm>
#include <string>

void Editor::Draw(Scene& scene, Camera& camera, Mesh& cubeMesh) {
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
			SelectEntity(scene, camera, i);
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

int Editor::GetSelectedEntity() const {
	return selectedEntity;
}

bool Editor::RayIntersectsCube(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& model, float& distance)
{
	glm::mat4 inverseModel = glm::inverse(model);

	//transforming the ray into the cube's local space
	glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(rayOrigin, 1.0f));
	glm::vec3 localDirection = glm::normalize(glm::vec3(inverseModel * glm::vec4(rayDirection, 0.0f)));

	glm::vec3 minBounds(-0.5f);
	glm::vec3 maxBounds(0.5f);
	float tMin = 0.0f;
	float tMax = 100000.0f;

	for (int axis = 0; axis < 3; axis++) {
		if (glm::abs(localDirection[axis]) < 0.000001f) {
			if (localOrigin[axis] < minBounds[axis] || localOrigin[axis] > maxBounds[axis]) {
				return false;
			}
		}
		else {
			float t1 = (minBounds[axis] - localOrigin[axis]) / localDirection[axis];
			float t2 = (maxBounds[axis] - localOrigin[axis]) / localDirection[axis];

			if (t1 > t2)
				std::swap(t1, t2);

			tMin = glm::max(tMin, t1);
			tMax = glm::min(tMax, t2);
			if (tMin > tMax)
				return false;

		}
	}
	
	if (tMax < 0.0f)
		return false;

	distance = (tMin >= 0.0f) ? tMin : tMax;
	return true;
}

void Editor::PickEntity(
	Scene& scene,
	Camera& camera,
	float mouseX,
	float mouseY,
	float screenWidth,
	float screenHeight)
{
	glm::vec3 rayOrigin;
	glm::vec3 rayDirection;

	camera.ScreenPointToRay(
		mouseX,
		mouseY,
		screenWidth,
		screenHeight,
		rayOrigin,
		rayDirection
	);

	int closestEntity = -1;
	float closestDistance = 100000.0f;

	for (int i = 0; i < scene.entities.size(); i++)
	{
		Entity& entity = scene.entities[i];

		glm::mat4 model =
			glm::translate(
				glm::mat4(1.0f),
				entity.transform.position
			);

		model = glm::rotate(
			model,
			glm::radians(entity.transform.rotation.x),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(entity.transform.rotation.y),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(entity.transform.rotation.z),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			entity.transform.scale
		);

		float distance;

		if (RayIntersectsCube(
			rayOrigin,
			rayDirection,
			model,
			distance))
		{
			if (distance < closestDistance)
			{
				closestDistance = distance;
				closestEntity = i;
			}
		}
	}

	SelectEntity(scene, camera, closestEntity);
}

void Editor::SelectEntity(Scene& scene, Camera& camera, int index) {
	if (index != selectedEntity) {
		if (transformMode != TransformMode::None)
		{
			CancelTransform(scene, camera);
		}
	}

	if (index < 0 || index >= static_cast<int>(scene.entities.size())) {
		selectedEntity = -1;
		transformMode = TransformMode::None;
		axisConstraint = AxisConstraint::None;
		return;
	}

	selectedEntity = index;
	camera.SetTarget(scene.entities[index].transform.position);
}

void Editor::BeginGrab(
	Scene& scene,
	Camera& camera,
	float mouseX,
	float mouseY)
{
	if (selectedEntity < 0 ||
		selectedEntity >= static_cast<int>(scene.entities.size()))
	{
		return;
	}

	Entity& entity = scene.entities[selectedEntity];

	transformMode = TransformMode::Grab;
	axisConstraint = AxisConstraint::None;

	originalPosition = entity.transform.position;
	grabStartPosition = entity.transform.position;
	grabStartMouse = glm::vec2(mouseX, mouseY);

	// Save the camera target so Escape can restore it.
	originalCameraTarget = camera.GetTarget();

	// Create a plane facing the camera, passing through the cube.
	grabPlaneNormal = glm::normalize(camera.GetTarget() - camera.GetPosition());

	ImVec2 screen = ImGui::GetIO().DisplaySize;

	glm::vec3 rayOrigin;
	glm::vec3 rayDirection;

	camera.ScreenPointToRay(
		mouseX,
		mouseY,
		screen.x,
		screen.y,
		rayOrigin,
		rayDirection
	);

	// Record the initial intersection for relative movement.
	if (!RayPlaneIntersection(
		rayOrigin,
		rayDirection,
		grabStartPosition,
		grabPlaneNormal,
		grabStartIntersection))
	{
		transformMode = TransformMode::None;
	}
}

void Editor::BeginRotate(Scene& scene, Camera& camera) {
	if (selectedEntity < 0 || selectedEntity >= static_cast<int>(scene.entities.size())) {
		return;
	}

	Entity& entity = scene.entities[selectedEntity];

	transformMode = TransformMode::Rotate;
	axisConstraint = AxisConstraint::None;

	originalRotation = entity.transform.rotation;
	rotateStartRotation = entity.transform.rotation;

	rotateStartMouseX = ImGui::GetMousePos().x;
}

void Editor::ApplyGrabMovement(Scene& scene, Camera& camera) {
	if (transformMode != TransformMode::Grab)
		return;

	if (selectedEntity < 0 || selectedEntity >= static_cast<int>(scene.entities.size()))
		return;

	Entity& entity = scene.entities[selectedEntity];

	ImVec2 mouse = ImGui::GetMousePos();
	ImVec2 screen = ImGui::GetIO().DisplaySize;

	glm::vec3 rayOrigin;
	glm::vec3 rayDirection;

	camera.ScreenPointToRay(mouse.x, mouse.y, screen.x, screen.y, rayOrigin, rayDirection);

	glm::vec3 currentIntersection;

	if (!RayPlaneIntersection(
		rayOrigin,
		rayDirection,
		grabStartPosition,
		grabPlaneNormal,
		currentIntersection))
	{
		return;
	}

	glm::vec3 movement =
		currentIntersection - grabStartIntersection;

	// Apply an optional world-axis constraint.
	if (axisConstraint != AxisConstraint::None)
	{
		glm::vec3 axis(0.0f);

		if (axisConstraint == AxisConstraint::X)
			axis.x = 1.0f;
		else if (axisConstraint == AxisConstraint::Y)
			axis.y = 1.0f;
		else if (axisConstraint == AxisConstraint::Z)
			axis.z = 1.0f;

		glm::vec3 projectedAxis =
			axis - glm::dot(axis, grabPlaneNormal) * grabPlaneNormal;

		float axisLengthSquared =
			glm::dot(projectedAxis, projectedAxis);

		// The axis is almost perpendicular to the plane's
		// usable movement direction.
		if (axisLengthSquared < 0.0001f)
			return;

		float amount =
			glm::dot(movement, projectedAxis) / axisLengthSquared;

		movement = axis * amount;
	}

	entity.transform.position = grabStartPosition + movement;
}

void Editor::ConfirmTransform() {
	transformMode = TransformMode::None;
	axisConstraint = AxisConstraint::None;
}

void Editor::CancelTransform(Scene& scene, Camera& camera)
{
	if (transformMode == TransformMode::None)
		return;

	if (selectedEntity >= 0 &&
		selectedEntity < static_cast<int>(scene.entities.size()))
	{
		Entity& entity = scene.entities[selectedEntity];

		if (transformMode == TransformMode::Grab)
			entity.transform.position = originalPosition;
		else if (transformMode == TransformMode::Rotate)
			entity.transform.rotation = originalRotation;
		else if (transformMode == TransformMode::Scale)
			entity.transform.scale = originalScale;
	}

	camera.SetTarget(originalCameraTarget);

	transformMode = TransformMode::None;
	axisConstraint = AxisConstraint::None;
}

void Editor::UpdateTransform(Scene& scene, Camera& camera)
{
	if (transformMode == TransformMode::None) {
		if (ImGui::IsKeyPressed(ImGuiKey_G) && !ImGui::GetIO().WantCaptureKeyboard) {
			ImVec2 mouse = ImGui::GetMousePos();
			BeginGrab(scene, camera, mouse.x, mouse.y);
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_R) && !ImGui::GetIO().WantCaptureKeyboard) {
			BeginRotate(scene, camera);
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_S) && !ImGui::GetIO().WantCaptureKeyboard) {
			BeginScale(scene, camera);
		}

		return;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_X))
		axisConstraint = AxisConstraint::X;
	else if (ImGui::IsKeyPressed(ImGuiKey_Y))
		axisConstraint = AxisConstraint::Y;
	else if (ImGui::IsKeyPressed(ImGuiKey_Z))
		axisConstraint = AxisConstraint::Z;

	if (ImGui::IsKeyPressed(ImGuiKey_Enter))
	{
		if (selectedEntity >= 0 &&
			selectedEntity < static_cast<int>(scene.entities.size()))
		{
			camera.SetTarget(
				scene.entities[selectedEntity].transform.position
			);
		}

		ConfirmTransform();
		return;
	}

	if (transformMode == TransformMode::Grab)
		ApplyGrabMovement(scene, camera);
	else if (transformMode == TransformMode::Rotate)
		ApplyRotateMovement(scene);
	else if (transformMode == TransformMode::Scale)
		ApplyScaleMovement(scene);
}

bool Editor::RayPlaneIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& planePoint, const glm::vec3& planeNormal, glm::vec3& intersection) {
	float denominator = glm::dot(rayDirection, planeNormal);

	if (glm::abs(denominator) < 0.0001f)
		return false;

	float t = glm::dot(planePoint - rayOrigin, planeNormal) / denominator;

	if (t < 0.0f)
		return false;

	intersection = rayOrigin + t * rayDirection;
	return true;
}

bool Editor::IsTransforming() const {
	return transformMode != TransformMode::None;
}

void Editor::ApplyRotateMovement(Scene& scene)
{
	if (transformMode != TransformMode::Rotate)
		return;

	if (selectedEntity < 0 ||
		selectedEntity >= static_cast<int>(scene.entities.size()))
	{
		return;
	}

	Entity& entity = scene.entities[selectedEntity];

	float currentMouseX = ImGui::GetMousePos().x;
	float mouseDelta = currentMouseX - rotateStartMouseX;

	// Convert horizontal mouse movement into degrees.
	float angle = mouseDelta * 0.5f;

	if (axisConstraint == AxisConstraint::X)
		entity.transform.rotation.x = rotateStartRotation.x + angle;
	else if (axisConstraint == AxisConstraint::Y)
		entity.transform.rotation.y = rotateStartRotation.y + angle;
	else if (axisConstraint == AxisConstraint::Z)
		entity.transform.rotation.z = rotateStartRotation.z + angle;
	else
	{
		// Default rotation: rotate around the world Y axis.
		entity.transform.rotation.y = rotateStartRotation.y + angle;
	}
}

void Editor::BeginScale(Scene& scene, Camera& camera) {
	if (selectedEntity < 0 || selectedEntity >= static_cast<int>(scene.entities.size())) {
		return;
	}

	Entity& entity = scene.entities[selectedEntity];

	transformMode = TransformMode::Scale;
	axisConstraint = AxisConstraint::None;

	originalScale = entity.transform.scale;
	scaleStartScale = entity.transform.scale;
	scaleStartMouseX = ImGui::GetMousePos().x;

	originalCameraTarget = camera.GetTarget();
}

void Editor::ApplyScaleMovement(Scene& scene) {
	if (transformMode != TransformMode::Scale)
		return;

	if (selectedEntity < 0 || selectedEntity >= static_cast<int>(scene.entities.size())) {
		return;
	}

	Entity& entity = scene.entities[selectedEntity];

	float currentMouseX = ImGui::GetMousePos().x;
	float mouseDelta = currentMouseX - scaleStartMouseX;

	float factor = glm::max(0.01f, 1.0f + mouseDelta * 0.01f);

	glm::vec3 newScale = scaleStartScale;

	if (axisConstraint == AxisConstraint::X)
		newScale.x *= factor;
	else if (axisConstraint == AxisConstraint::Y)
		newScale.y *= factor;
	else if (axisConstraint == AxisConstraint::Z)
		newScale.z *= factor;
	else
		newScale *= factor;

	entity.transform.scale = newScale;
}