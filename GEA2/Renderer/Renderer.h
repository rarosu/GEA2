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
class Renderer
{
public:

	Renderer(Camera* camera, ChunkManager* pchunkManager, unsigned width, unsigned height);
	~Renderer();

	void Draw();

	void Resize(unsigned width, unsigned height);

	bool& GetSSAOFlag();

private:

	ShaderProgram shaderProgram;
	ShaderProgram output;
	ShaderProgram outputWithSSAOBlur;

	Mesh	fullscreenquad;
	Camera* camera;
	Buffer	cameraBuffer;
	GBuffer gbuffer;
	ChunkManager* chunkManager;
	Texture texture;
	SSAO	ssao;
	bool SSAOEnabled;
	GLuint vao;
};