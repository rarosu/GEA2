#pragma once

#include <glew.h>
#include <glm.hpp>
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Camera.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkManager.h"
#include "Texture.h"

class Renderer
{
public:
	Renderer(Camera* camera);
	~Renderer();

	void Draw();

	//Quick and dirty block destroyer, remove if needed
	void DestroyBlock();
	//Return pointer to chunkychunk
	ChunkManager* GetChunkManager();
private:

	ShaderProgram shaderProgram;
	Camera* camera;
	Buffer cameraBuffer;

	//Test chunkmanager
	ChunkManager chunkyChunkMan;
	Texture texture;
};