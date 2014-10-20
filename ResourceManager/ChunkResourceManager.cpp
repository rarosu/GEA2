#include "ChunkResourceManager.h"
#include <sstream>
#include "RLE/rle.c"
#include <iostream>
#include "../GEA2/Config.h"


ChunkResourceManager::ChunkResourceManager(Filesystem* filesystem, MemoryAllocator* _allocator, const std::string& vWorldPath)
	: allocator(_allocator->GetInterface("ChunkResourceManager"))
{
	currentVWorldPath = vWorldPath;

	this->filesystem = filesystem;

	file = filesystem->GetFile(currentVWorldPath);
	
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

	//Print world data
	std::cout << "World '" << vWorldPath << "' header read!" << std::endl;
	std::cout << "Chunks: x: " << global_header.SCX << " y: " << global_header.SCY << " z: " << global_header.SCZ << std::endl;
	std::cout << "Blocks: x: " << global_header.CX << " y: " << global_header.CY << " z: " << global_header.CZ << std::endl;
	std::cout << "Total chunks: " << global_header.SCX * global_header.SCY * global_header.SCZ << std::endl;
	unsigned int blocks = global_header.SCX * global_header.SCY * global_header.SCZ * global_header.CX * global_header.CY * global_header.CZ - 1;
	std::cout << "Total blocks: " << blocks << std::endl;

	header = new header_element[global_header.SCX * global_header.SCY * global_header.SCZ];
	file->Read(header, global_header.header_size - sizeof(MetaWorldHeader));

	size_t chunkSize = global_header.CX * global_header.CY * global_header.CZ + sizeof(Chunk);

	chunkMem = allocator.Alloc(chunkSize * MAX_CHUNKS_IN_MEM);

	pool = new(allocator.Alloc(sizeof(ThreadedPoolAllocator))) ThreadedPoolAllocator(chunkMem, chunkSize, MAX_CHUNKS_IN_MEM);
}
ChunkResourceManager::~ChunkResourceManager()
{
	allocator.Free(chunkMem);
	allocator.Free(pool);

	delete[] header;
}

void ChunkResourceManager::Destructor(InternalResource<Chunk>* internal)
{
	pool->Free(internal->resource);

	chunks.RemoveResource(internal->hash);
}

Resource<Chunk> ChunkResourceManager::Get(int x, int y, int z)
{
	std::hash<std::string> hasher;
	std::stringstream ss;
	ss << currentVWorldPath << ',' << x << ',' << y << ',' << z;
	auto hash = hasher(ss.str());

	auto internal = chunks.GetResource(hash);
	if (internal != nullptr)
		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));

	return Resource<Chunk>();
}

Resource<Chunk> ChunkResourceManager::Load(int x, int y, int z)
{
	std::hash<std::string> hasher;
	std::stringstream ss;

	std::string filename(currentVWorldPath);

	ss << filename << ',' << x << ',' << y << ',' << z;
	auto hash = hasher(ss.str());

	auto internal = chunks.GetResource(hash);
	if (internal != nullptr)
	{
		std::cerr << "Resource found." << std::endl;
		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));
	}
		

	{
		if (file == nullptr)
		{
			std::cerr << "File not open." << std::endl;
			return Resource<Chunk>();
		}


		struct header_element
		{
			int address;
			int size;
		};

		int i = globalFileHeader.SCZ * globalFileHeader.SCY * x + globalFileHeader.SCZ * y + z;

		//Create array, seek to chunk in file, read compressed data to mem
		uint8_t* compressed = new(std::nothrow) uint8_t[header[i].size];
		if (compressed == nullptr)
			return Resource<Chunk>();

		{
			std::lock_guard<std::mutex> lock(mutex);

			file->Seek(header[i].address, File::Origin::ORIGIN_BEG);
			file->Read(compressed, header[i].size);
		}

		char* chunkMem = (char*)pool->Alloc();
		Chunk* chunk = new(chunkMem)Chunk(chunkMem + sizeof(Chunk), glm::vec3(x * globalFileHeader.CX, y * globalFileHeader.CY, z * globalFileHeader.CZ), globalFileHeader);
		
		RLE_Uncompress(compressed, (unsigned char*)chunk->blockList, header[i].size);
		chunk->changed = true;

		delete[] compressed;

		internal = chunks.AddResource(hash, chunk);
		if (internal == nullptr)
		{
			std::cerr << "Collision with hash." << std::endl;
			return Resource<Chunk>();
		}
			
		//std::cerr << "Resource created." << std::endl;
		return Resource<Chunk>(internal, std::bind(&ChunkResourceManager::Destructor, this, std::placeholders::_1));
	}
}

const MetaWorldHeader& ChunkResourceManager::GetGlobalWorldHeader()
{
	return globalFileHeader;
}
