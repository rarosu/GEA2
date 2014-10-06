#pragma once

#include <glew.h>
#include <glm.hpp>
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Camera.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkManager.h"
#include "Texture.h"
#include "GBuffer.h"

class Renderer
{
public:
	Renderer(Camera* camera, unsigned width, unsigned height);
	~Renderer();

	void Draw();

	void Resize(unsigned width, unsigned height);

	//Quick and dirty block destroyer, remove if needed
	void DestroyBlock();
	//Return pointer to chunkychunk
	ChunkManager* GetChunkManager();

private:

	ShaderProgram shaderProgram;
	ShaderProgram output;

	Mesh	fullscreenquad;
	Camera* camera;
	Buffer	cameraBuffer;
	GBuffer gbuffer;

	//Test chunkmanager
	ChunkManager chunkyChunkMan;
	Texture texture;
};