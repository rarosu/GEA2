#include "Renderer.h"

Renderer::Renderer(Camera* camera)
	: camera(camera), cameraBuffer(GL_UNIFORM_BUFFER)
{
	shaderProgram.CreateProgram("../Assets/Shaders/Cube");

	//Test stuff
	//////////////////////////////////////////////////////////////////////////
	Vertex points[] = 
	{
		Vertex(glm::vec4(-0.5f, -0.5f,  0.0f, 1.0f), glm::vec3(0), glm::vec2(0)),
		Vertex(glm::vec4(0.5f, -0.5f,  0.0f, 1.0f), glm::vec3(0), glm::vec2(0)),
		Vertex(glm::vec4(0.0f,  0.5f,  0.0f, 1.0f), glm::vec3(0), glm::vec2(0))
	};

	testMesh.CreateVertexBuffer(points, 3);
	//////////////////////////////////////////////////////////////////////////

	//Set up camera buffer
	cameraBuffer.BufferData(1, sizeof(glm::mat4), &camera->GetViewProjMatrix(), GL_DYNAMIC_DRAW);
}

Renderer::~Renderer()
{

}

void Renderer::Draw()
{
	//Test
	cameraBuffer.BufferSubData(0, sizeof(glm::mat4), &camera->GetViewProjMatrix());
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer.GetBufferId());

	shaderProgram.Use();
	testMesh.Draw();
	//////////////////
}

