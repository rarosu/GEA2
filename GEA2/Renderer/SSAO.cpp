#include "SSAO.h"
#include <ctime>

SSAO::SSAO(ShaderResourceManager* shaderManager)
	: kernelBuffer(GL_UNIFORM_BUFFER)
{
	this->shaderManager = shaderManager;
}

SSAO::~SSAO()
{
	glDeleteTextures(1, &noiseTexture);
}


void SSAO::Init(unsigned windowWidth, unsigned windowHeight)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &outTexture);
	glBindTexture(GL_TEXTURE_2D, outTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, outTexture, 0);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	glGenTextures(1, &noiseTexture);

	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	const int noiseSize = 16;
	glm::vec2 noise[noiseSize];
	for (int i = 0; i < noiseSize; ++i) {
		noise[i] = glm::vec2(
			Random(-1.0f, 1.0f),
			Random(-1.0f, 1.0f));
		noise[i] = glm::normalize(noise[i]);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 4, 4, 0, GL_RG, GL_FLOAT, &noise[0]);

	srand(static_cast <unsigned> (time(0)));
	int kernelSize = 16;
	for (int i = 0; i < kernelSize; ++i)
	{
		kernelSamples[i] = glm::vec4(
			Random(-1.0f, 1.0f),
			Random(-1.0f, 1.0f),
			Random(0.0f, 1.0f),
			0.0f);
		kernelSamples[i] = glm::normalize(kernelSamples[i]);
		float scale = float(i) / float(kernelSize);
		scale = glm::mix<float>(0.1f, 1.0f, scale * scale);
		kernelSamples[i] *= scale;
	}

	kernelBuffer.BufferData((size_t)kernelSize, sizeof(glm::vec4), &kernelSamples[0], GL_DYNAMIC_DRAW);

	ssaoShader = shaderManager->Load("SSAO");
}


float SSAO::Random(float p_low, float p_high)
{
	return p_low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (p_high - p_low)));
}

void SSAO::Bind()
{
	ssaoShader->Use();
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, kernelBuffer.GetBufferId());
}

void SSAO::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void SSAO::BindTexForRead(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, outTexture);
}

void SSAO::UnbindTexForRead(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SSAO::Resize(unsigned windowWidth, unsigned windowHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindTexture(GL_TEXTURE_2D, outTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTexture, 0);
}

