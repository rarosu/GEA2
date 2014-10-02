#pragma once

#include <glew.h>
#include <glm.hpp>
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Camera.h"

class Renderer
{
public:
	Renderer(Camera* camera);
	~Renderer();

	void Draw();

private:

	ShaderProgram shaderProgram;
	Mesh testMesh;
	Camera* camera;
	Buffer cameraBuffer;
};