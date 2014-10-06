#pragma once

#include <glew.h>
#include <glm.hpp>
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Camera.h"
#include "Chunk/ChunkManager.h"
#include "Texture.h"
#include "GBuffer.h"

class Renderer
{
public:
	Renderer(Camera* camera, ChunkManager* pchunkManager, unsigned width, unsigned height);
	~Renderer();

	void Draw();

	void Resize(unsigned width, unsigned height);



private:

	ShaderProgram shaderProgram;
	ShaderProgram output;

	Mesh	fullscreenquad;
	Camera* camera;
	Buffer	cameraBuffer;
	GBuffer gbuffer;
	ChunkManager* chunkManager;
	Texture texture;
};