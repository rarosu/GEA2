#include "ChunkResourceManager.h"
#include <sstream>
#include "RLE/rle.c"

ChunkResourceManager::ChunkResourceManager(Filesystem* filesystem)
{
	this->filesystem = filesystem;
	file = filesystem->GetFile("samesize.world");
	
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
	MetaWorldHeader global_header;
	file->Read(&global_header, sizeof(MetaWorldHeader));
	globalFileHeader = global_header;

	header = new header_element[global_header.SCX * global_header.SCY * global_header.SCZ];
	file->Read(header, global_header.header_size - sizeof(MetaWorldHeader));
}
ChunkResourceManager::~ChunkResourceManager()
{
	delete[] header;
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
	std::string filename("samesize.world");
	ss << filename << ',' << x << ',' << y << ',' << z;
	auto hash = hasher(ss.str());

	auto internal = chunks.GetResource(hash);
	if (internal != nullptr)
		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));

	{
		if (file == nullptr)
			return Resource<Chunk>();

		struct header_element
		{
			int address;
			int size;
		};

		int i = globalFileHeader.SCZ * globalFileHeader.SCY * x + globalFileHeader.SCZ * y + z;

		//Create array, seek to chunk in file, read compressed data to mem
		uint8_t* compressed = new uint8_t[header[i].size];
		file->Seek(header[i].address, File::Origin::ORIGIN_BEG);
		file->Read(compressed, header[i].size);

		//Create a chunk and uncompress data, set chunk to changed
		Chunk* chunk = new Chunk(glm::vec3(x * globalFileHeader.CX, y * globalFileHeader.CY, z * globalFileHeader.CZ), globalFileHeader);
		RLE_Uncompress(compressed, (unsigned char*)chunk->blockList, header[i].size);

		chunk->changed = true;

		delete[] compressed;

		internal = chunks.AddResource(hash, chunk);
		if (internal == nullptr)
			return Resource<Chunk>();

		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));
	}
}

const MetaWorldHeader& ChunkResourceManager::GetGlobalWorldHeader()
{
	return globalFileHeader;
}
