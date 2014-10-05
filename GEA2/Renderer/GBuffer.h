#pragma once
#include <glew.h>

#define GBUFFER_NUM_TEXTURES 2

class GBuffer
{
public:

	enum GBUFFER_TEXTURE
	{
		GBUFFER_TEXTURE_COLOR,
		GBUFFER_TEXTURE_NORMALS,
	};

	GBuffer();
	~GBuffer();

	bool Init(unsigned windowWidth, unsigned windowHeight);
	void Resize(unsigned windowWidth, unsigned windowHeight);
	void Bind();
	void BindTextures();
	void UnbindTextures();

private:

	GLuint fbo;
	GLuint textures[GBUFFER_NUM_TEXTURES];
	GLuint depthTexture;

};