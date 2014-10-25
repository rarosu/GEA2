#include "Chunk.h"
#include <iostream>
#include <gtc/noise.hpp>
#include <gtc/matrix_transform.hpp>
#include <ChunkResourceManager.h>

Chunk::Chunk(char* blockListMemory, const glm::vec3& worldPos, const MetaWorldHeader& metaHeader)
: changed(true), numberOfElements(0), left(nullptr), right(nullptr), below(nullptr), above(nullptr), front(nullptr), back(nullptr), metaWorldHeader(metaHeader), vertexBuffer(GL_ARRAY_BUFFER)
{
	worldMatrix = glm::translate(worldMatrix, worldPos);
	//Calculate world position only when chunk is created, no need for world matrix! WOOOO!
	worldP = glm::vec4(worldPos.x , worldPos.y, worldPos.z , 0.0f);
	//Set all blocks in chunk to 0 (air), this causes the mesh generation to not create any vertices, see UpdateChunk() for deatails
	blockList = new(blockListMemory) uint8_t[metaWorldHeader.CX * metaWorldHeader.CY * metaWorldHeader.CZ];
	
	memset(blockList, 0, sizeof(uint8_t) * metaWorldHeader.CX * metaWorldHeader.CY * metaWorldHeader.CZ);

	//Precalculate UV-coords for each block type(should probably be in chunkmanager to avoid duplicate memory storage)

	for (uint8_t type = 0; type < 16; ++type)
	{
		///Calculate correct UV mapped to atlas
		//UV coord size for a block side on the texture atlas
		glm::vec2 blockUVSize = glm::vec2(1.0f / ATLASTEXTURES_X, 1.0f / ATLASTEXTURES_Y);

		//2D index of where on the atlas the block type has its texture part
		glm::ivec2 textureAtlasPosition = glm::ivec2(type % ATLASTEXTURES_X, type / ATLASTEXTURES_Y);

		//Calculate absolute UV coords for the 3 different block parts, top, side and bottom. These are used to sample the atlas directly in the shader
		//UV for top face
		x0[type] = textureAtlasPosition.x * blockUVSize.x;
		x1[type] = x0[type] + blockUVSize.x;

		y0[type] = textureAtlasPosition.y * blockUVSize.y;
		y1[type] = y0[type] + blockUVSize.y;

		//UV for side faces
		ys0[type] = y1[type];
		ys1[type] = ys0[type] + blockUVSize.y;

		//UV for bottom faces
		yb0[type] = ys1[type];
		yb1[type] = yb0[type] + blockUVSize.y;
	}
}

Chunk::~Chunk()
{

}

void Chunk::UpdateChunk(std::mutex* mutex)
{
	changed = false;

	Vertex* vertices = new Vertex[metaWorldHeader.CX * metaWorldHeader.CY * metaWorldHeader.CZ * 36];
	int i = 0;
	int merged = 0;
	bool vis = false;

	//Merge building
	// View from negative x
	for (int8_t x = metaWorldHeader.CX - 1; x >= 0; --x)
	{
		for (int8_t y = 0; y < metaWorldHeader.CY; ++y)
		{
			for (int8_t z = 0; z < metaWorldHeader.CZ; ++z)
			{
				uint8_t blockType = blockList[GetBlockArrayIndex(x, y, z)];
				
				//Air block or existing neighbour block? Don't build face.
				if (blockType == 0 || Get(x - 1, y, z) != 0)
				{
					vis = false;
					continue;
				}

				uint8_t blkUV = blockType - 1;
				glm::vec3 normal(-1, 0, 0);

				
				if (vis && z != 0 && blockList[GetBlockArrayIndex(x, y, z)] == blockList[GetBlockArrayIndex(x, y, z - 1)])
				{
					vertices[i - 5] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys1[blkUV]));
					vertices[i - 2] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys1[blkUV]));
					vertices[i - 1] = Vertex(glm::vec4(x, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					++merged;
				}
				else
				{
					//add world position here instead of world matrix multiplication for every vertex every frame
					vertices[i] = Vertex(glm::vec4(x, y, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					vertices[i + 1] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys1[blkUV]));
					vertices[i + 2] = Vertex(glm::vec4(x, y + 1, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys0[blkUV]));
					vertices[i + 3] = Vertex(glm::vec4(x, y + 1, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys0[blkUV]));
					vertices[i + 4] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys1[blkUV]));
					vertices[i + 5] = Vertex(glm::vec4(x, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					i += 6;
				}

				vis = true;
			}
		}
	}

	//Merge building
	// View from positive x
	for (int8_t x = 0; x < metaWorldHeader.CX; ++x)
	{
		for (int8_t y = 0; y < metaWorldHeader.CY; ++y)
		{
			for (int8_t z = 0; z < metaWorldHeader.CZ; ++z)
			{
				uint8_t blockType = blockList[GetBlockArrayIndex(x, y, z)];

				//Air block or existing neighbour block? Don't build face.
				if (blockType == 0 || Get(x + 1, y, z) != 0)
				{
					vis = false;
					continue;
				}

				uint8_t blkUV = blockType - 1;
				glm::vec3 normal(1, 0, 0);

				if (vis && z != 0 && blockList[GetBlockArrayIndex(x, y, z)] == blockList[GetBlockArrayIndex(x, y, z - 1)])
				{
					vertices[i - 4] = Vertex(glm::vec4(x + 1, y, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					vertices[i - 2] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys0[blkUV]));
					vertices[i - 1] = Vertex(glm::vec4(x + 1, y, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					++merged;
				}
				else
				{
					//add world position here instead of world matrix multiplication for every vertex every frame
					vertices[i] = Vertex(glm::vec4(x + 1, y, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys1[blkUV]));
					vertices[i + 1] = Vertex(glm::vec4(x + 1, y + 1, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					vertices[i + 2] = Vertex(glm::vec4(x + 1, y, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					vertices[i + 3] = Vertex(glm::vec4(x + 1, y + 1, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					vertices[i + 4] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys0[blkUV]));
					vertices[i + 5] = Vertex(glm::vec4(x + 1, y, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					i += 6;
				}

				vis = true;
			}
		}
	}

	//Merge building
	// View from negative y
	for (int8_t x = 0; x < metaWorldHeader.CX; ++x)
	{
		for (int8_t y = metaWorldHeader.CY - 1; y >= 0; --y)
		{
			for (int8_t z = 0; z < metaWorldHeader.CZ; ++z)
			{
				uint8_t blockType = blockList[GetBlockArrayIndex(x, y, z)];

				//Air block or existing neighbour block? Don't build face.
				if (blockType == 0 || Get(x, y - 1, z) != 0)
				{
					vis = false;
					continue;
				}

				uint8_t blkUV = blockType - 1;
				glm::vec3 normal(0, -1, 0);

				if (vis && z != 0 && blockList[GetBlockArrayIndex(x, y, z)] == blockList[GetBlockArrayIndex(x, y, z - 1)])
				{
					vertices[i - 4] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], yb0[blkUV]));
					vertices[i - 2] = Vertex(glm::vec4(x + 1, y, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], yb0[blkUV]));
					vertices[i - 1] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], yb0[blkUV]));
					++merged;
				}
				else
				{
					//add world position here instead of world matrix multiplication for every vertex every frame
					vertices[i] = Vertex(glm::vec4(x, y, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], yb1[blkUV]));
					vertices[i + 1] = Vertex(glm::vec4(x + 1, y, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], yb1[blkUV]));
					vertices[i + 2] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], yb0[blkUV]));
					vertices[i + 3] = Vertex(glm::vec4(x + 1, y, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], yb1[blkUV]));
					vertices[i + 4] = Vertex(glm::vec4(x + 1, y, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], yb0[blkUV]));
					vertices[i + 5] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], yb0[blkUV]));
					i += 6;
				}

				vis = true;
			}
		}
	}

	//Merge building
	// View from positive y
	for (int8_t x = 0; x < metaWorldHeader.CX; ++x)
	{
		for (int8_t y = 0; y < metaWorldHeader.CY; ++y)
		{
			for (int8_t z = 0; z < metaWorldHeader.CZ; ++z)
			{
				uint8_t blockType = blockList[GetBlockArrayIndex(x, y, z)];

				//Air block or existing neighbour block? Don't build face.
				if (blockType == 0 || Get(x, y + 1, z) != 0)
				{
					vis = false;
					continue;
				}

				uint8_t blkUV = blockType - 1;
				glm::vec3 normal(0, 1, 0);

				if (vis && z != 0 && blockList[GetBlockArrayIndex(x, y, z)] == blockList[GetBlockArrayIndex(x, y, z - 1)])
				{
					vertices[i - 6] = Vertex(glm::vec4(x, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], y0[blkUV]));
					vertices[i - 5] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], y0[blkUV]));
					vertices[i - 3] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], y0[blkUV]));
					++merged;
				}
				else
				{
					//add world position here instead of world matrix multiplication for every vertex every frame
					vertices[i] = Vertex(glm::vec4(x, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], y0[blkUV]));
					vertices[i + 1] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], y0[blkUV]));
					vertices[i + 2] = Vertex(glm::vec4(x, y + 1, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], y1[blkUV]));
					vertices[i + 3] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], y0[blkUV]));
					vertices[i + 4] = Vertex(glm::vec4(x + 1, y + 1, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], y1[blkUV]));
					vertices[i + 5] = Vertex(glm::vec4(x, y + 1, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], y1[blkUV]));
					i += 6;
				}

				vis = true;
			}
		}
	}

	//Merge building
	// View from negative z
	for (int8_t y = 0; y < metaWorldHeader.CY; ++y)
	{
		for (int8_t z = metaWorldHeader.CZ - 1; z >= 0; --z)
		{
			for (int8_t x = 0; x < metaWorldHeader.CX; ++x)
			{
				uint8_t blockType = blockList[GetBlockArrayIndex(x, y, z)];

				//Air block or existing neighbour block? Don't build face.
				if (blockType == 0 || Get(x, y, z - 1) != 0)
				{
					vis = false;
					continue;
				}

				uint8_t blkUV = blockType - 1;
				glm::vec3 normal(0, 0, -1);

				if (vis && x != 0 && blockList[GetBlockArrayIndex(x, y, z)] == blockList[GetBlockArrayIndex(x - 1, y, z)])
				{
					vertices[i - 4] = Vertex(glm::vec4(x + 1, y, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					vertices[i - 2] = Vertex(glm::vec4(x + 1, y + 1, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys0[blkUV]));
					vertices[i - 1] = Vertex(glm::vec4(x + 1, y, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					++merged;
				}
				else
				{
					//add world position here instead of world matrix multiplication for every vertex every frame
					vertices[i] = Vertex(glm::vec4(x, y, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys1[blkUV]));
					vertices[i + 1] = Vertex(glm::vec4(x, y + 1, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					vertices[i + 2] = Vertex(glm::vec4(x + 1, y, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					vertices[i + 3] = Vertex(glm::vec4(x, y + 1, z, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					vertices[i + 4] = Vertex(glm::vec4(x + 1, y + 1, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys0[blkUV]));
					vertices[i + 5] = Vertex(glm::vec4(x + 1, y, z, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					i += 6;
				}

				vis = true;
			}
		}
	}

	//Merge building
	// View from positive z
	for (int8_t y = 0; y < metaWorldHeader.CY; ++y)
	{
		for (int8_t z = 0; z < metaWorldHeader.CZ; ++z)
		{
			for (int8_t x = 0; x < metaWorldHeader.CX; ++x)
			{
				uint8_t blockType = blockList[GetBlockArrayIndex(x, y, z)];

				//Air block or existing neighbour block? Don't build face.
				if (blockType == 0 || Get(x, y, z + 1) != 0)
				{
					vis = false;
					continue;
				}

				uint8_t blkUV = blockType - 1;
				glm::vec3 normal(0, 0, 1);

				if (vis && x != 0 && blockList[GetBlockArrayIndex(x, y, z)] == blockList[GetBlockArrayIndex(x - 1, y, z)])
				{
					vertices[i - 6] = Vertex(glm::vec4(x + 1, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys1[blkUV]));
					vertices[i - 5] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					vertices[i - 3] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					++merged;
				}
				else
				{
					//add world position here instead of world matrix multiplication for every vertex every frame
					vertices[i] = Vertex(glm::vec4(x + 1, y, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys1[blkUV]));
					vertices[i + 1] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					vertices[i + 2] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					vertices[i + 3] = Vertex(glm::vec4(x + 1, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x1[blkUV], ys0[blkUV]));
					vertices[i + 4] = Vertex(glm::vec4(x, y + 1, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys0[blkUV]));
					vertices[i + 5] = Vertex(glm::vec4(x, y, z + 1, blockType) + worldP, normal, glm::vec2(x0[blkUV], ys1[blkUV]));
					i += 6;
				}

				vis = true;
			}
		}
	}

	numberOfElements = i;

	//Don't create mesh if all blocks are air
	if (numberOfElements == 0)
	{
		delete[] vertices;
		return;
	}

	std::lock_guard<std::mutex> lock(*mutex);

	vertexBuffer.BufferData(numberOfElements, sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Synch.
	GLsync fenceId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	GLenum result;
	while (true)
	{
		result = glClientWaitSync(fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(5000000000)); //5 Second timeout
		if (result != GL_TIMEOUT_EXPIRED) break; //we ignore timeouts and wait until all OpenGL commands are processed!
	}

	glDeleteSync(fenceId);

	delete[] vertices;
}

void Chunk::Set( int x, int y, int z, uint8_t type )
{
	//If coordinates are outside this chunk, find the right neighbour chunk and tell it to change the right block.
	if(x < 0) 
	{
		if(left)
			left->Set(x + metaWorldHeader.CX, y, z, type);
		return;
	}
	if(x >= metaWorldHeader.CX) 
	{
		if(right)
			right->Set(x - metaWorldHeader.CX, y, z, type);
		return;
	}
	if(y < 0) 
	{
		if(below)
			below->Set(x, y + metaWorldHeader.CY, z, type);
		return;
	}
	if(y >= metaWorldHeader.CY) 
	{
		if(above)
			above->Set(x, y - metaWorldHeader.CY, z, type);
		return;
	}
	if(z < 0) 
	{
		if(front)
			front->Set(x, y, z + metaWorldHeader.CZ, type);
		return;
	}
	if(z >= metaWorldHeader.CZ) 
	{
		if(back)
			back->Set(x, y, z - metaWorldHeader.CZ, type);
		return;
	}

	// Change the block
	blockList[GetBlockArrayIndex(x, y, z)] = type;
	changed = true;

	// When updating blocks at the edge of this chunk,
	// visibility of blocks in the neighbouring chunk might change.
	if(x == 0 && left)
		left->changed = true;
	if(x == metaWorldHeader.CX - 1 && right)
		right->changed = true;
	if(y == 0 && below)
		below->changed = true;
	if(y == metaWorldHeader.CY - 1 && above)
		above->changed = true;
	if(z == 0 && front)
		front->changed = true;
	if(z == metaWorldHeader.CZ - 1 && back)
		back->changed = true;
}

uint8_t Chunk::Get( int x, int y, int z )
{
	//Get block type at position, if outside of this chunk, get from neighbour
	int index = 0;
	if (x < 0)
	{
		index = GetBlockArrayIndex(x + metaWorldHeader.CX, y, z);
		return left ? left->blockList[index] : 0;
	}
	if (x >= metaWorldHeader.CX)
	{
		index = GetBlockArrayIndex(x - metaWorldHeader.CX, y, z);
		return right ? right->blockList[index] : 0;
	}
	if (y < 0)
	{
		index = GetBlockArrayIndex(x, y + metaWorldHeader.CY, z);
		return below ? below->blockList[index] : 0;
	}
	if(y >= metaWorldHeader.CY)
	{
		index = GetBlockArrayIndex(x, y - metaWorldHeader.CY, z);
		return above ? above->blockList[index] : 0;
	}
	if (z < 0)
	{
		index = GetBlockArrayIndex(x, y, z + metaWorldHeader.CZ);
		return front ? front->blockList[index] : 0;
	}
	if (z >= metaWorldHeader.CZ)
	{
		index = GetBlockArrayIndex(x, y, z - metaWorldHeader.CZ);
		return back ? back->blockList[index] : 0;
	}

	return blockList[GetBlockArrayIndex(x, y, z)];
}

int Chunk::GetBlockArrayIndex(int x, int y, int z)
{
	return metaWorldHeader.CZ * metaWorldHeader.CY * x + metaWorldHeader.CZ * y + z;
}
