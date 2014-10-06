#include "Buffer.h"

Buffer::Buffer( GLenum type )
{
	this->type = type;
	glGenBuffers(1, &id);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &id);
}

void Buffer::BufferData( size_t elementCount, size_t elementSize, void* data, GLuint usage )
{
	glBindBuffer(type, id);
	glBufferData(type, elementCount * elementSize, data, usage);

	this->elementSize = elementSize;
	this->numElements = elementCount;
	size = elementCount * elementSize;
}

void Buffer::BufferSubData( size_t offset, size_t length, void* data )
{
	glBindBuffer(type, id);
	glBufferSubData(type, offset, length, data);
}

void* Buffer::MapBuffer( GLenum access )
{
	glBindBuffer(type, id);
	return glMapBuffer(type, access);
}

bool Buffer::UnmapBuffer()
{
	GLboolean errCheck = glUnmapBuffer(type);
	glBindBuffer(type, 0);

	if(errCheck == GL_TRUE)
		return true;
	else 
		return false;
}

GLuint Buffer::GetBufferId() const
{
	return id;
}

GLenum Buffer::GetBufferType() const
{
	return type;
}

size_t Buffer::GetBufferSize() const
{
	return size;
}

size_t Buffer::GetElementSize() const
{
	return elementSize;
}

size_t Buffer::GetNumElements() const
{
	return numElements;
}

