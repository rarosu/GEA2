#include "ChunkManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "../../Config.h"

ChunkManager::ChunkManager(Filesystem* filesystem, MemoryAllocator* allocator, Camera* pcamera, const std::string& vWorldPath)
: chunkResManager(filesystem, allocator, vWorldPath), camera(pcamera), chunkLoadPool(CHUNK_LOAD_THREADS), metaHeader(chunkResManager.GetGlobalWorldHeader()), previousTotalChunkCount(0)
{
	chunkLoadDistance = DEFAULT_CHUNK_LOAD_DISTANCE;
	nrOfRenderedChunks = 0;
	drawList.reserve(MAX_CHUNKS_IN_MEM);
	
	camera->SetPosition(glm::vec3(metaHeader.CX * metaHeader.SCX * 0.5f,
								  68.0f,
								  metaHeader.CZ * metaHeader.SCZ * 0.5f));

	doOnce = false;
	taskAddedCount = 0;
}

ChunkManager::~ChunkManager()
{
}

void ChunkManager::Update(float dt)
{
	if (!doOnce && taskAddedCount != 0 && chunkFutures.size() == 0)
	{
		double t = taskTimer.Stop() / 1000.0f;
		std::cout << "Chunks per second: " << (taskAddedCount / t) << std::endl;
		doOnce = true;
	}

	//Get chunk position
	int cx = (int)camera->GetPosition().x / metaHeader.CX;
	int cy = (int)camera->GetPosition().y / metaHeader.CY;
	int cz = (int)camera->GetPosition().z / metaHeader.CZ;

	//Get lower chunk iteration pos
	int lowX = std::max(cx - (chunkLoadDistance + 1), 0);
	int lowY = std::max(cy - (chunkLoadDistance + 1), 0);
	int lowZ = std::max(cz - (chunkLoadDistance + 1), 0);

	//Get upper chunk iteration pos
	int highX = std::min(cx + (chunkLoadDistance + 1), metaHeader.SCX);
	int highY = std::min(cy + (chunkLoadDistance + 1), metaHeader.SCY);
	int highZ = std::min(cz + (chunkLoadDistance + 1), metaHeader.SCZ);

	// Check for loaded chunks to add to the draw list.
	std::vector<std::pair<int, std::future<Resource<Chunk>>>>::iterator it;
	for (it = chunkFutures.begin(); it != chunkFutures.end();)
	{
		if (it->second._Is_ready())
		{
			Resource<Chunk> chunk = it->second.get();
			drawList.push_back(chunk);
			existMap[it->first] = chunk;

			it = chunkFutures.erase(it);
		
		}
		else
		{
			it++;
		}
	}
	nrOfTasks = chunkFutures.size();

	//looping over high and low creates a box around the camera containing possible loaded/unloaded chunks. Go through these and do load/unload logic. 
	//(Previously we looped through 500k chunks in a 512x2x512 level, this approach takes it down to worst case ish (chunkLoadDistance*2)^3 and in the case of 512x2x512 the worst case is (chunkLoadDistance*2)^2 * 2)
	for (int x = lowX; x < highX; ++x)
		for (int y = lowY; y < highY; ++y)
			for (int z = lowZ; z < highZ; ++z)
			{
				float dist = glm::distance(glm::vec3(x*metaHeader.CX, y*metaHeader.CY, z*metaHeader.CZ), camera->GetPosition());
				if (dist < metaHeader.CX * chunkLoadDistance)
				{
					AddChunk(x, y, z);
				}
			}	

	for (auto it = existMap.begin(); it != existMap.end();)
	{
		if (it->second == nullptr)
		{
			it++;
			continue;
		}

		glm::vec3 displacement = camera->GetPosition() - glm::vec3(it->second->worldP);

		float dist = glm::dot(displacement, displacement);
		if (dist > (chunkLoadDistance*metaHeader.CX * chunkLoadDistance*metaHeader.CX))
		{
			// Check if there is a chunk loaded
			auto itdraw = std::find(drawList.begin(), drawList.end(), it->second);
			drawList.erase(itdraw);
			it = existMap.erase(it);
		}
		else
			it++;
	}
}

void ChunkManager::Draw()
{
	nrOfRenderedChunks = 0;
	for (auto chunk : drawList)
	{
		glm::mat4 WVP = camera->GetViewProjMatrix() * chunk->worldMatrix;
		glm::vec4 center = WVP * glm::vec4(metaHeader.CX / 2, metaHeader.CY / 2, metaHeader.CZ / 2, 1);
		center.x /= center.w;
		center.y /= center.w;

		if (center.z < -metaHeader.CZ / 2)
			continue;
		if (fabsf(center.x) > 1 + fabsf(metaHeader.CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(metaHeader.CY * 2 / center.w))
			continue;
		if (!chunk->numberOfElements)
			continue;
		++nrOfRenderedChunks;
		glBindVertexBuffer(0, chunk->vertexBuffer.GetBufferId(), 0, chunk->vertexBuffer.GetElementSize());

		glDrawArrays(GL_TRIANGLES, 0, chunk->numberOfElements);
		
	}	
}

uint8_t ChunkManager::Get(int x, int y, int z)
{
	int cx = x / metaHeader.CX;
	int cy = y / metaHeader.CY;
	int cz = z / metaHeader.CZ;

	int pos = metaHeader.SCZ * metaHeader.SCY * cx + metaHeader.SCZ * cy + cz;
	auto itmap = existMap.find(pos);
	if (itmap != existMap.end())
	{
		if (itmap->second == nullptr)
			return 0;

		x %= metaHeader.CX;
		y %= metaHeader.CY;
		z %= metaHeader.CZ;

		return itmap->second->Get(x, y, z);
	}

	return 0;
}

uint8_t ChunkManager::Get(const glm::vec3& pos)
{
	return Get((int)pos.x, (int)pos.y, (int)pos.z);
}

void ChunkManager::Set( int x, int y, int z, uint8_t type )
{
	int cx = x / metaHeader.CX;
	int cy = y / metaHeader.CY;
	int cz = z / metaHeader.CZ;

	int pos = metaHeader.SCZ * metaHeader.SCY * cx + metaHeader.SCZ * cy + cz;
	auto itmap = existMap.find(pos);
	if (itmap != existMap.end())
	{
		if (itmap->second == nullptr)
			return;

		x %= metaHeader.CX;
		y %= metaHeader.CY;
		z %= metaHeader.CZ;

		itmap->second->Set(x, y, z, type);
	}
}

int& ChunkManager::GetNrOfRenderedChunks()
{
	return nrOfRenderedChunks;
}

int& ChunkManager::GetViewRadius()
{
	return chunkLoadDistance;
}

int& ChunkManager::GetNrOfBlocks()
{
	int vertices = 0;
	for (auto d : drawList)
	{
		vertices += d->numberOfElements;
	}
	std::cout << "Number of rendered vertices: " << vertices << std::endl;
	return nrOfBlocks;
}

int ChunkManager::GetNrOfChunks()
{
	return nrOfChunks;
}

int& ChunkManager::GetNrOfTasks()
{
	return nrOfTasks;
}

void ChunkManager::AddChunk(int x, int y, int z)
{
	int pos = metaHeader.SCZ * metaHeader.SCY * x + metaHeader.SCZ * y + z;
	if (existMap.find(pos) == existMap.end())
	{
		int totalChunkCount = drawList.size() + chunkFutures.size();

		if (totalChunkCount < MAX_CHUNKS_IN_MEM)
		{
			chunkFutures.push_back(
				std::pair<int, std::future<Resource<Chunk>>>(
				pos, chunkLoadPool.AddTask<LoadChunkTask>(&mutex, &chunkResManager, x, y, z, this)));

			existMap[pos] = Resource<Chunk>();
			taskAddedCount++;

			if (taskAddedCount == 1)
			{
				// start timer
				taskTimer.Start();
			}
		}
		else if (previousTotalChunkCount < MAX_CHUNKS_IN_MEM)
		{
			std::cerr << "Chunk limit reached! Number of chunks: " << drawList.size() << std::endl;
		}

		previousTotalChunkCount = totalChunkCount;
	}
}


void ChunkManager::RemoveChunk(int x, int y, int z)
{
	

	// TODO: Check if there is a chunk currently being loaded and in that case abort the task.
}

void ChunkManager::DestroyBlock()
{
	//Ray march 10 half blocks forward from camera position, check for non air blocks and destroy
	glm::vec3 pos = camera->GetPosition();
	glm::vec3 dir = glm::normalize(camera->GetFacing());

	for (int i = 0; i < 10; ++i)
	{
		if (Get((int)pos.x, (int)pos.y, (int)pos.z) != 0)
		{
			Set((int)pos.x, (int)pos.y, (int)pos.z, 0);
			break;
		}
		pos += dir*0.5f;
	}
}

std::map<int, Resource<Chunk>>* ChunkManager::GetExistMap()
{
	return &existMap;
}
