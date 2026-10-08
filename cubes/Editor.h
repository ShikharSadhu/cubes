#pragma once
class Scene;
class Mesh;

class Editor {
public:
	void Draw(Scene& scene, Mesh& cubeMesh);
private:
	int selectedEntity = -1;
	void DrawInspector(Scene& scene);
};