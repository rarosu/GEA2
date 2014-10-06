#pragma once

#include <glew.h>

class Buffer
{
public:

	Buffer(GLenum type);
	~Buffer();

	void BufferData(size_t elementCount, size_t elementSize, void* data, GLuint usage);
	void BufferSubData(size_t offset, size_t length, void* data);

	GLuint GetBufferId() const;
	GLenum GetBufferType() const;
	size_t GetBufferSize() const;
	size_t GetElementSize() const;
	size_t GetNumElements() const;
	void* MapBuffer(GLenum access);
	bool UnmapBuffer();
private:

	GLuint id;
	GLenum type;
	size_t size;
	size_t elementSize;
	size_t numElements;
};