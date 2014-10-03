#pragma once

#include <glew.h>
#include <glm.hpp>
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Camera.h"
#include "Chunk/Chunk.h"

class Renderer
{
public:
	Renderer(Camera* camera);
	~Renderer();

	void Draw();

private:

	ShaderProgram shaderProgram;
	Camera* camera;
	Buffer cameraBuffer;

	//Test chunk
	Chunk chunkychunk;
};