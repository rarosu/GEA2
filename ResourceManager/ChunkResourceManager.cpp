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
	std::string filename("testtest.world");
	ss << filename << ',' << x << ',' << y << ',' << z;
	auto hash = hasher(ss.str());

	auto internal = chunks.GetResource(hash);
	if (internal != nullptr)
		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));

	{

		std::shared_ptr<File> file = filesystem->GetFile("testtest.world");

		if (file == nullptr)
			return Resource<Chunk>();

		struct header_element
		{
			int address;
			int size;
		};

		struct header_global
		{
			int header_size;
			int SCX;
			int SCY;
			int SCZ;
			int CX;
			int CY;
			int CZ;
		};

		//Header format
		//-header_global	- 0 bytes
		//---header_size	- 0 bytes
		//---SCX			- 4 bytes
		//---SCY			- 8 bytes
		//---SCZ			- 12 bytes
		//---CX				- 16 bytes
		//---CY				- 20 bytes
		//---CZ				- 24 bytes
		//-header_element	- 28 bytes
		//		-
		//		-
		//		-
		//-start of compressed data - header_global.header_size bytes
		
		//Read global header data!
		header_global global_header;
		file->Read(&global_header, sizeof(header_global));
		//Read header elements
		header_element* header = new header_element[SCX * SCY * SCZ];
		file->Read(header, global_header.header_size - sizeof(header_global));

		int i = SCZ * SCY * x + SCZ * y + z;

		//Create array, seek to chunk in file, read compressed data to mem
		uint8_t* compressed = new uint8_t[header[i].size];
		file->Seek(header[i].address, File::Origin::ORIGIN_BEG);
		file->Read(compressed, header[i].size);

		//Create a chunk and uncompress data, set chunk to changed
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