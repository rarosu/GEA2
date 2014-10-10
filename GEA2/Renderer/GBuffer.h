#pragma once
#include <glew.h>

#define GBUFFER_NUM_TEXTURES 4

class GBuffer
{
public:

	enum GBUFFER_TEXTURE
	{
		GBUFFER_TEXTURE_COLOR,
		GBUFFER_TEXTURE_NORMALS,
		GBUFFER_TEXTURE_POSITION,
		GBUFFER_TEXTURE_COLOR_OUT,
		GBUFFER_TEXTURE_DEPTH,
	};

	GBuffer();
	~GBuffer();

	bool Init(unsigned windowWidth, unsigned windowHeight);
	void Resize(unsigned windowWidth, unsigned windowHeight);
	void Bind();
	void Unbind();
	void BindTexture(GBUFFER_TEXTURE tex);
	void UnbindTexture(GBUFFER_TEXTURE tex);
	GLuint& GetTextureHandle(GBUFFER_TEXTURE tex);

private:

	GLuint fbo;
	GLuint textures[GBUFFER_NUM_TEXTURES];
	GLuint depthTexture;

};