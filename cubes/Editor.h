#pragma once
#include <glm/glm.hpp>

class Scene;
class Mesh;
class Camera;
struct GLFWwindow;

class Editor {
public:
	void Draw(Scene& scene, Camera& camera, Mesh& cubeMesh);
	int GetSelectedEntity() const;
	void PickEntity(Scene& scene, Camera& camera, float mouseX, float mouseY, float screenWidth, float screenHeight);
	void SelectEntity(Scene& scene, Camera& camera, int index);		//for the camera
	void BeginGrab(Scene& scene, Camera& camera, float mouseX, float mouseY);
	void UpdateTransform(Scene& scene, Camera& camera);
	void ConfirmTransform();
	void CancelTransform(Scene& scene, Camera& camera);
	bool IsTransforming() const;
	void BeginRotate(Scene& scene, Camera& camera);
	void BeginScale(Scene& scene, Camera& camera);
private:
	int selectedEntity = -1;
	void DrawInspector(Scene& scene);
	bool RayIntersectsCube(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::mat4& model, float& distance);
	void ApplyGrabMovement(Scene& scene, Camera& camera);
	bool RayPlaneIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& planePoint, const glm::vec3& planeNormal, glm::vec3& intersection);
	void ApplyRotateMovement(Scene& scene);
	void ApplyScaleMovement(Scene& scene);

	enum class TransformMode {
		None,
		Grab,
		Rotate,
		Scale
	};

	enum class AxisConstraint {
		None,
		X,
		Y,
		Z
	};

	TransformMode transformMode = TransformMode::None;
	AxisConstraint axisConstraint = AxisConstraint::None;

	glm::vec3 originalPosition{ 0.0f };
	glm::vec3 grabStartPosition{ 0.0f };
	glm::vec2 grabStartMouse{ 0.0f };
	glm::vec3 grabStartIntersection{ 0.0f };
	glm::vec3 grabPlaneNormal{ 0.0f };
	glm::vec3 originalCameraTarget{ 0.0f };
	glm::vec3 originalRotation{ 0.0f };
	glm::vec3 rotateStartRotation{ 0.0f };
	float rotateStartMouseX = 0.0f;
	glm::vec3 originalScale{ 1.0f };
	glm::vec3 scaleStartScale{ 1.0f };
	float scaleStartMouseX = 0.0f;
};