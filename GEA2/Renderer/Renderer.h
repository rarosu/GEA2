#pragma once

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

	Renderer(Camera* camera, ChunkManager* pchunkManager, unsigned width, unsigned height);
	~Renderer();

	void Update(float dt);
	void Draw();

	void Resize(unsigned width, unsigned height);

	bool& GetSSAOFlag();
private:

	ShaderProgram	chunkProgram;
	ShaderProgram	output;
	ShaderProgram	outputWithSSAOBlur;

	Mesh			fullscreenquad;
	Camera*			camera;
	Buffer			cameraBuffer;
	GBuffer			gbuffer;
	ChunkManager*	chunkManager;
	Texture			textureAtlas;
	SSAO			ssao;
	Water			water;

	bool			SSAOEnabled;
};