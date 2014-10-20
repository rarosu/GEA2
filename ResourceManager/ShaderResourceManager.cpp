#include "ShaderResourceManager.h"

ShaderResourceManager::ShaderResourceManager(Filesystem* filesystem, MemoryAllocator* _allocator)
: allocator(_allocator->GetInterface("ProgramResourceManager"))
{
	this->filesystem = filesystem;
}

ShaderResourceManager::~ShaderResourceManager()
{

}

void ShaderResourceManager::Destructor(InternalResource<ShaderProgram>* internal)
{
	allocator.Free(internal->resource);
	textures.RemoveResource(internal->hash);
}

Resource<ShaderProgram> ShaderResourceManager::Load(const std::string& vpath)
{
	std::hash<std::string> hasher;
	size_t hash = hasher(vpath);

	InternalResource<ShaderProgram>* internal = textures.GetResource(hash);
	if (internal != nullptr)
		return Resource<ShaderProgram>(internal, std::bind(&ShaderResourceManager::Destructor, this, std::placeholders::_1));

	char* filedata;
	size_t filesize;

	{
		std::shared_ptr<File> file = filesystem->GetFile(vpath + ".sp");

		if (file == nullptr)
			return Resource<ShaderProgram>();

		filesize = file->GetFileSize();

		filedata = (char*)allocator.Alloc(filesize);
		file->Read(filedata, filesize);
	}

	// Create program.
	ShaderProgram* program = (ShaderProgram*)allocator.Alloc(sizeof(ShaderProgram));
	program->CreateProgram(vpath.c_str(), filedata, filesize, filesystem);

	allocator.Free(filedata);

	internal = textures.AddResource(hash, program);
	if (internal == nullptr)
		return Resource<ShaderProgram>();

	return Resource<ShaderProgram>(internal, std::bind(&ShaderResourceManager::Destructor, this, std::placeholders::_1));
}