#pragma once
#include <glew.h>
#include <glm.hpp>
#include "Buffer.h"
#include "ShaderProgram.h"

class SSAO
{
public:
	SSAO();
	~SSAO();

	void Init(unsigned windowWidth, unsigned windowHeight);
	void Bind();
	void Unbind();

	void BindTexForRead(int slot);
	void UnbindTexForRead(int slot);
private:

	float Random(float p_low, float p_high);

	glm::vec4 kernelSamples[16];
	Buffer	kernelBuffer;
	GLuint noiseTexture;
	GLuint outTexture;
	GLuint fbo;

	ShaderProgram ssaoShader;
};