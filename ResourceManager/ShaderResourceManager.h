#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "Resource.h"
#include "ResourceContainer.h"
#include "Filesystem.h"
#include "MemoryAllocator.h"
#include "../GEA2/Renderer/ShaderProgram.h"

#include <glew.h>
#include <mutex>

class ProgramResourceManager
{
PUBLIC:
	ProgramResourceManager(Filesystem* filesystem, MemoryAllocator* allocator);
	~ProgramResourceManager();

	Resource<ShaderProgram> Load(const std::string& vpath);
	void Destructor(InternalResource<ShaderProgram>* internal);
PRIVATE:
	ResourceContainer<ShaderProgram> textures;
	Filesystem* filesystem;
	MemoryAllocatorInterface allocator;
};

class ShaderResourceManager
{
PUBLIC:
	ShaderResourceManager(Filesystem* filesystem, MemoryAllocator* allocator);
	~ShaderResourceManager();

	Resource<ShaderProgram> Load(const std::string& vpath);
	void Destructor(InternalResource<ShaderProgram>* internal);
PRIVATE:
	ResourceContainer<ShaderProgram> textures;
	Filesystem* filesystem;
	MemoryAllocatorInterface allocator;
};