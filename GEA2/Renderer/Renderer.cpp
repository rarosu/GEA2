#include "Renderer.h"

Renderer::Renderer()
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
}

Renderer::~Renderer()
{

}

void Renderer::Draw()
{
	//Test
	shaderProgram.Use();
	testMesh.Draw();
	//////////////////
}

