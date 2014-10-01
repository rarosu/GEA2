#pragma once

#include <glew.h>
#include "ShaderProgram.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Draw();

private:

	ShaderProgram shaderProgram;

	//test
	GLuint vao;
};