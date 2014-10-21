#pragma once

#include "../../ResourceManager/ShaderResourceManager.h"
#include <glew.h>
#include <glm.hpp>
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Camera.h"
#include "Chunk/ChunkManager.h"
#include "Texture.h"
#include "GBuffer.h"
#include "SSAO.h"
#include "Water.h"

class Renderer
{
public:

	Renderer(Camera* camera, ChunkManager* pchunkManager, unsigned width, unsigned height, Filesystem* filesystem, MemoryAllocator* memoryAllocator);
	~Renderer();

	void Update(float dt);
	void Draw();

	void Resize(unsigned width, unsigned height);

	bool& GetSSAOFlag();
	bool& GetWaterFlag();

private:

	ShaderResourceManager shaderManager;
	Resource<ShaderProgram>	chunkProgram;
	Resource<ShaderProgram>	output;
	Resource<ShaderProgram>	outputWithSSAOBlur;

	Mesh			fullscreenquad;
	Camera*			camera;
	Buffer			cameraBuffer;
	GBuffer			gbuffer;
	ChunkManager*	chunkManager;
	Texture			textureAtlas;
	SSAO			ssao;
	Water			water;

	bool			SSAOEnabled;
	bool			waterEnabled;

	GLuint vao;
	
};