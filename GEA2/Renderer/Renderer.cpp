#include "Renderer.h"

Renderer::Renderer()
{
	shaderProgram.CreateProgram("../Shaders/Cube");
}

Renderer::~Renderer()
{

}

void Renderer::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

