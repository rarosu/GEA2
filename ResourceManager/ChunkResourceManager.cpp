#include "ChunkResourceManager.h"
#include <sstream>
#include "RLE/rle.c"

ChunkResourceManager::ChunkResourceManager(Filesystem* filesystem)
{
	this->filesystem = filesystem;
}
ChunkResourceManager::~ChunkResourceManager()
{

}

void ChunkResourceManager::Destructor(InternalResource<Chunk>* internal)
{
	delete internal->resource;
	chunks.RemoveResource(internal->hash);
}

Resource<Chunk> ChunkResourceManager::Load(int x, int y, int z)
{
	std::hash<std::string> hasher;
	std::stringstream ss;
	std::string filename("world.world");
	ss << filename << ',' << x << ',' << y << ',' << z;
	auto hash = hasher(ss.str());

	auto internal = chunks.GetResource(hash);
	if (internal != nullptr)
		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));

	{

		std::shared_ptr<File> file = filesystem->GetFile("world.world");

		if (file == nullptr)
			return Resource<Chunk>();

		struct header_element
		{
			int address;
			int size;
		};

		header_element* header = new header_element[SCX * SCY * SCZ];

		int header_size;
		file->Read(&header_size, sizeof(int));
		file->Read(header, header_size - sizeof(int));
		int i = SCZ * SCY * x + SCZ * y + z;
		uint8_t* compressed = new uint8_t[header[i].size];
		file->Seek(header[i].address, File::Origin::ORIGIN_BEG);
		file->Read(compressed, header[i].size);
		Chunk* chunk = new Chunk(glm::vec3(x * CX, y * CY, z * CZ));
		RLE_Uncompress(compressed, (unsigned char*)chunk->blockList, header[i].size);
		chunk->changed = true;

		delete[] compressed;
		delete[] header;

		internal = chunks.AddResource(hash, chunk);
		if (internal == nullptr)
			return Resource<Chunk>();

		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));
	}
}