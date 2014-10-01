#include "Renderer.h"

Renderer::Renderer()
{
	shaderProgram.CreateProgram("../Shaders/Cube");

	//Test stuff
	//////////////////////////////////////////////////////////////////////////
	float points[] = 
	{
		-0.5f, -0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
		0.0f,  0.5f,  0.0f
	};

	GLuint vbo = 0;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), points, GL_STATIC_DRAW);

	vao = 0;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//////////////////////////////////////////////////////////////////////////
}

Renderer::~Renderer()
{

}

void Renderer::Draw()
{
	shaderProgram.Use();
	//Test
	glBindVertexArray (vao);
	glDrawArrays (GL_TRIANGLES, 0, 3);
	//////////////////
}

