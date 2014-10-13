#include "Water.h"
#include <iostream>

Water::Water()
	: timeBuf(GL_UNIFORM_BUFFER), time(0)
{

}

Water::~Water()
{

}

void Water::Init(int windowWidth, int windowHeight, GLuint depthTex, GLuint colorTex)
{
	inDepthTex = depthTex;
	inColorTex = colorTex;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inColorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, inDepthTex, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "GL_FRAMEBUFFER error, status: " << status << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	waterShader.CreateProgram("../Assets/Shaders/Water");

	float waterheight = 12.5f;
	Vertex waterQuadV[] =
	{
		Vertex(glm::vec4(-10000, waterheight, -10000, 1), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
		Vertex(glm::vec4(-10000, waterheight, 10000, 1), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
		Vertex(glm::vec4(10000, waterheight, -10000, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1)),
		Vertex(glm::vec4(10000, waterheight, -10000, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1)),
		Vertex(glm::vec4(-10000, waterheight, 10000, 1), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
		Vertex(glm::vec4(10000, waterheight, 10000, 1), glm::vec3(0, 1, 0), glm::vec2(1, 1))
	};

	//Create fullscreen quad mesh
	waterQuad.CreateVertexBuffer(&waterQuadV[0], 6);

	timeBuf.BufferData(1, sizeof(float), 0, GL_DYNAMIC_DRAW);

	normalTexture.Load("../Assets/watertex.png");
	normalTexture.Bind(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	normalTexture.Unbind(0);
}

void Water::Draw()
{
	Bind();
	normalTexture.Bind(0);
	waterShader.Use();
	timeBuf.BufferSubData(0, sizeof(float), &time);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, timeBuf.GetBufferId());
	waterQuad.Draw();
	normalTexture.Unbind(0);
	Unbind();
}

void Water::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inColorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, inDepthTex, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
}

void Water::Unbind()
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glActiveTexture(GL_TEXTURE0 + 4);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Water::BindFullscreenTexForRead(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, inColorTex);
}

void Water::UnbindFullscreenTexForRead(int slot)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Water::Resize(int windowWidth, int windowHeight)
{
	/*glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glBindTexture(GL_TEXTURE_2D, outTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTexture, 0);
	*/

}

void Water::Update(float dt)
{
	time += dt;
}
