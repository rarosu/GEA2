#include "VertexAttributes.h"

VertexAttributes::VertexAttributes()
	: stride(0)
{
}

VertexAttributes::~VertexAttributes()
{
	glDeleteVertexArrays(1, &vao);
}

void VertexAttributes::Bind()
{
	glBindVertexArray(vao);
}

void VertexAttributes::Unbind()
{
	glBindVertexArray(0);
}

void VertexAttributes::Init( size_t numAttribs )
{
	glGenVertexArrays(1, &vao);
	Bind();

	for(size_t i = 0; i < numAttribs; ++i) 
	{
		glEnableVertexAttribArray(i);
	}

	this->numAttribs = numAttribs;
	Unbind();
}

void VertexAttributes::SetVertexAttribPointer( GLuint bufferId, GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer )
{
	Bind();
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	if(type == GL_UNSIGNED_INT)
		glVertexAttribIPointer(location, size, type, stride, pointer);
	else
		glVertexAttribPointer(location, size, type, normalized, stride, pointer);
	Unbind();
}

size_t VertexAttributes::GetNumAttributes() const
{
	return numAttribs;
}

void VertexAttributes::SetFormat( GLuint location, GLint elementCount, GLint elementSize, GLenum type, GLenum normalized, GLuint offset )
{
	Bind();
	glVertexAttribFormat(location, elementCount, type, normalized, offset);

	stride += elementCount * elementSize;
	Unbind();
}

void VertexAttributes::SetVertexBuffer( GLuint bufferId, GLuint offset )
{
	Bind();
	for(size_t i = 0; i < numAttribs; ++i) 
	{
		glBindVertexBuffer(0, bufferId, offset, stride);
		glVertexAttribBinding(i, 0);
	}
	Unbind();
}

void VertexAttributes::SetVertexBuffer( GLuint bufferId, GLuint bindingIndex, GLuint location, GLuint offset )
{
	Bind();
	glBindVertexBuffer(bindingIndex, bufferId, offset, this->stride);
	glVertexAttribBinding(location, bindingIndex);
	Unbind();
}

size_t VertexAttributes::GetStride() const
{
	return stride;
}
