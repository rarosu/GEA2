#include "ChunkManager.h"
#include <fstream>
#include <iostream>

ChunkManager::ChunkManager(Filesystem* filesystem, Camera* pcamera)
: worldMatBuf(GL_UNIFORM_BUFFER), chunkResManager(filesystem), camera(pcamera)
{
	worldMatBuf.BufferData(1, sizeof(glm::mat4), 0, GL_DYNAMIC_DRAW);
}


ChunkManager::~ChunkManager()
{
}

void ChunkManager::Update(float dt)
{
	for (int x = 0; x < SCX; ++x)
		for (int y = 0; y < SCY; ++y)	
			for (int z = 0; z < SCZ; ++z)
			{
				float dist = glm::distance(glm::vec3(x*CX, y*CY, z*CZ), camera->GetPosition());
				if (dist < 16 * 20)
				{
					AddChunk(x, y, z);
				}
				else
				{
					RemoveChunk(x, y, z);
				}
			}	
}

void ChunkManager::Draw()
{
	for (auto chunk : drawList)
	{
		worldMatBuf.BufferSubData(0, sizeof(glm::mat4), &chunk->worldMatrix);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, worldMatBuf.GetBufferId());

		chunk->Draw();
	}			
}

uint8_t ChunkManager::Get(int x, int y, int z)
{
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	int pos = SCZ * SCY * cx + SCZ * cy + cz;
	auto itmap = existMap.find(pos);
	if (itmap != existMap.end())
	{
		x %= CX;
		y %= CY;
		z %= CZ;

		return itmap->second->Get(x, y, z);
	}

	return 0;
}

void ChunkManager::Set( int x, int y, int z, uint8_t type )
{
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	int pos = SCZ * SCY * cx + SCZ * cy + cz;
	auto itmap = existMap.find(pos);
	if (itmap != existMap.end())
	{
		x %= CX;
		y %= CY;
		z %= CZ;

		itmap->second->Set(x, y, z, type);
	}
}

int& ChunkManager::GetNrOfBlocks()
{
	return nrOfBlocks;
}

int ChunkManager::GetNrOfChunks()
{
	return nrOfChunks;
}

void ChunkManager::AddChunk(int x, int y, int z)
{
	int pos = SCZ * SCY * x + SCZ * y + z;
	if (existMap.find(pos) == existMap.end())
	{
		Resource<Chunk> chunk = chunkResManager.Load(x, y, z);
		
		drawList.push_back(chunk);
		existMap[pos] = chunk;
	}
}

void ChunkManager::RemoveChunk(int x, int y, int z)
{
	int pos = SCZ * SCY * x + SCZ * y + z;
	auto itmap = existMap.find(pos);
	if (itmap != existMap.end())
	{
		auto itdraw = std::find(drawList.begin(), drawList.end(), itmap->second);
		drawList.erase(itdraw);
		existMap.erase(pos);
	}
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