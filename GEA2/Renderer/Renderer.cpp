#include "Renderer.h"

Renderer::Renderer(Camera* camera)
	: camera(camera), cameraBuffer(GL_UNIFORM_BUFFER)
{
	shaderProgram.CreateProgram("../Assets/Shaders/Cube");

	//Set up camera buffer
	cameraBuffer.BufferData(1, sizeof(glm::mat4), &camera->GetViewProjMatrix(), GL_DYNAMIC_DRAW);

	//Testing chunk
	chunkychunk.Noise(0, 0, 0, 0);

	texture.Load("../Assets/testBlockTex.png");
}

Renderer::~Renderer()
{

}

void Renderer::Draw()
{
	cameraBuffer.BufferSubData(0, sizeof(glm::mat4), &camera->GetViewProjMatrix());
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer.GetBufferId());

	texture.Bind(0);
	shaderProgram.Use();
	//Testing chunkychunk
	chunkychunk.Draw();

	texture.Unbind(0);
}

