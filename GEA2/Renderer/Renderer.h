#pragma once

#include <glew.h>
#include <glm.hpp>
#include "ShaderProgram.h"
#include "Mesh.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Draw();

private:

	ShaderProgram shaderProgram;
	Mesh testMesh;
};