#include "ChunkManager.h"


ChunkManager::ChunkManager()
	: worldMatBuf(GL_UNIFORM_BUFFER)
{
	memset(chunkList, 0, sizeof(chunkList));

	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCY; y++)
			for(int z = 0; z < SCZ; z++)
				chunkList[x][y][z] = new Chunk(glm::vec3(x * CX, y * CY, z * CZ));

	worldMatBuf.BufferData(1, sizeof(glm::mat4), &chunkList[0][0][0]->worldMatrix, GL_DYNAMIC_DRAW);

	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCY; y++)
			for(int z = 0; z < SCZ; z++) 
			{
				if(x > 0)
					chunkList[x][y][z]->left = chunkList[x - 1][y][z];
				if(x < SCX - 1)
					chunkList[x][y][z]->right = chunkList[x + 1][y][z];
				if(y > 0)
					chunkList[x][y][z]->below = chunkList[x][y - 1][z];
				if(y < SCY - 1)
					chunkList[x][y][z]->above = chunkList[x][y + 1][z];
				if(z > 0)
					chunkList[x][y][z]->front = chunkList[x][y][z - 1];
				if(z < SCZ - 1)
					chunkList[x][y][z]->back = chunkList[x][y][z + 1];
			}

	GenerateTerrain();

	nrOfChunks = SCX*SCX*SCZ;
	nrOfBlocks = nrOfChunks*CX*CY*CZ;
}


ChunkManager::~ChunkManager()
{
	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCX; y++)
			for(int z = 0; z < SCX; z++)
				delete chunkList[x][y][z];
}

void ChunkManager::Draw()
{
	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCY; y++)
			for(int z = 0; z < SCZ; z++)
				if (chunkList[x][y][z])
				{
					worldMatBuf.BufferSubData(0, sizeof(glm::mat4), &chunkList[x][y][z]->worldMatrix);
					glBindBufferBase(GL_UNIFORM_BUFFER, 1, worldMatBuf.GetBufferId());

					chunkList[x][y][z]->Draw();
				}
}

uint8_t ChunkManager::Get(int x, int y, int z)
{
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;

	if(!chunkList[cx][cy][cz])
		return 0;
	else
		return chunkList[cx][cy][cz]->Get(x, y, z);
}

void ChunkManager::Set( int x, int y, int z, uint8_t type )
{
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;
	  
	if(!chunkList[cx][cy][cz])
	{
		nrOfChunks++;
		chunkList[cx][cy][cz] = new Chunk(glm::vec3(cx * CX, cy * CY, cz * CZ));
	}

	nrOfBlocks++;
	chunkList[cx][cy][cz]->Set(x, y, z, type);
}

int ChunkManager::GetNrOfBlocks()
{
	return nrOfBlocks;
}

int ChunkManager::GetNrOfChunks()
{
	return nrOfChunks;
}

void ChunkManager::GenerateTerrain()
{
	for (int x = 0; x < SCX; x++)
	{
		for(int y = 0; y < SCY; y++)
		{
			for (int z = 0; z < SCZ; z++)
			{
				chunkList[x][y][z]->Noise(1, x, y, z);
			}
		}
	}
}
