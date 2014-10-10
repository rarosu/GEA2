#pragma once
#include <glew.h>
#include <glm.hpp>
#include <string>
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Buffer.h"

class Water
{
public:

	Water();
	~Water();

	void Init(int windowWidth, int windowHeight, GLuint depthTex, GLuint colorTex);
	void Bind();
	void Unbind();
	void Draw();
	void BindFullscreenTexForRead(int slot);
	void UnbindFullscreenTexForRead(int slot);
	void Update(float dt);
	void Resize(int windowWidth, int windowHeight);

private:

	float time;

	GLuint fbo;
	GLuint outTexture;

	//These are from the gbuffer, will be attached to this fbo to continue as forward pass
	GLuint inDepthTex;
	GLuint inColorTex;

	Texture normalTexture;

	Buffer timeBuf;
	Mesh waterQuad;
	ShaderProgram waterShader;
};