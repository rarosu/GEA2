#pragma once

#include <glew.h>

class ShaderProgram
{
public:
	//Compiles and attaches shaders to program, performs CreateProgram directly. Maybe not that useful, but I won't remove it yet..
	ShaderProgram(const char* shaderProgramPath);
	ShaderProgram();
	~ShaderProgram();

	//Get shader handle
	GLuint GetHandle();

	//Compiles and attaches shaders to program
	bool CreateProgram(const char* shaderProgramPath);

	//Use program, could extend this to change GLstates like depth write and other things
	void Use();

private:

	//Creates and compiles a shader, called from CreateProgram automagically
	GLuint CreateShader(GLenum shaderType, const char* shaderPath);

	//Only data in this object
	GLuint programHandle;
};