#pragma once

#include <glew.h>

class VertexAttributes
{
public:
	VertexAttributes();
	~VertexAttributes();

	void	Bind();
	void	Unbind();
	void	Init(size_t numAttribs);
	void	SetVertexAttribPointer(GLuint bufferId, GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer);
	size_t	GetNumAttributes() const;
private:

	void	SetFormat(GLuint location, GLint elementCount, GLint elementSize, GLenum type, GLenum normalized, GLuint offset);	
	void	SetVertexBuffer(GLuint bufferId, GLuint offset);
	void	SetVertexBuffer(GLuint bufferId, GLuint bindingIndex, GLuint location, GLuint offset);
	size_t	GetStride() const;

	GLuint	vao;
	size_t	numAttribs;
	GLsizei stride;
};