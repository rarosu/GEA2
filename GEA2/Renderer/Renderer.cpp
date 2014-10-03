#include "Renderer.h"

Renderer::Renderer(Camera* camera)
	: camera(camera), cameraBuffer(GL_UNIFORM_BUFFER)
{
	shaderProgram.CreateProgram("../Assets/Shaders/Cube");

	//Set up camera buffer
	cameraBuffer.BufferData(1, sizeof(glm::mat4), &camera->GetViewProjMatrix(), GL_DYNAMIC_DRAW);

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
	chunkyChunkMan.Draw();

	texture.Unbind(0);
}

void Renderer::DestroyBlock()
{
	glm::vec3 pos = camera->GetPosition();
	glm::vec3 dir = glm::normalize(camera->GetFacing());

	for (int i = 0; i < 20; ++i)
	{
		if (chunkyChunkMan.Get((int)pos.x, (int)pos.y, (int)pos.z) != 0)
		{
			chunkyChunkMan.Set((int)pos.x, (int)pos.y, (int)pos.z, 0);
			break;
		}
		pos += dir*0.5f;
	}
}

