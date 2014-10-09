#include "Chunk.h"
#include <iostream>
#include <gtc/noise.hpp>
#include <gtc/matrix_transform.hpp>
#include <ChunkResourceManager.h>

Chunk::Chunk(const glm::vec3& worldPos, const MetaWorldHeader& metaHeader)
	: changed(true), numberOfElements(0), left(nullptr), right(nullptr), below(nullptr), above(nullptr), front(nullptr), back(nullptr), chunkMesh(nullptr), metaWorldHeader(metaHeader)
{
	worldMatrix = glm::translate(worldMatrix, worldPos);
	//Calculate world position only when chunk is created, no need for world matrix! WOOOO!
	worldP = glm::vec4(worldPos.x , worldPos.y, worldPos.z , 0.0f);
	//Set all blocks in chunk to 0 (air), this causes the mesh generation to not create any vertices, see UpdateChunk() for deatails
	blockList = new uint8_t[metaWorldHeader.CX * metaWorldHeader.CY * metaWorldHeader.CZ];
	
	memset(blockList, 0, sizeof(uint8_t) * metaWorldHeader.CX * metaWorldHeader.CY * metaWorldHeader.CZ);
}

Chunk::~Chunk()
{
	//Delete chunk mesh on chunk destruction
	if (chunkMesh)
		delete chunkMesh;

	delete[] blockList;
}

void Chunk::UpdateChunk()
{
	changed = false;

	std::vector<Vertex> renderList;

	bool vis = false;

	for(uint8_t x = 0; x < metaWorldHeader.CX; x++) 
	{
		for(uint8_t y = 0; y < metaWorldHeader.CY; y++) 
		{
			for(uint8_t z = 0; z < metaWorldHeader.CZ; z++) 
			{
				// Empty block?
				if (!blockList[GetBlockArrayIndex(x, y, z)])
				{
					vis = false;
					continue;
				}
				
				///Calculate correct UV mapped to atlas
				uint8_t blockType = blockList[GetBlockArrayIndex(x, y, z)];

				//UV coord size for a block side on the texture atlas
				glm::vec2 blockUVSize = glm::vec2(1.0f / ATLASTEXTURES_X, 1.0f / ATLASTEXTURES_Y);
				
				//2D index of where on the atlas the block type has its texture part
				glm::ivec2 textureAtlasPosition = glm::ivec2((blockType - 1) % ATLASTEXTURES_X, (blockType - 1) / ATLASTEXTURES_Y);

				//Calculate absolute UV coords for the 3 different block parts, top, side and bottom. These are used to sample the atlas directly in the shader
				//UV for top face
				float x0 = textureAtlasPosition.x * blockUVSize.x;
				float x1 = x0 + blockUVSize.x;

				float y0 = textureAtlasPosition.y * blockUVSize.y;
				float y1 = y0 + blockUVSize.y;

				//UV for side faces
				float ys0 = y1;
				float ys1 = ys0 + blockUVSize.y;

				//UV for bottom faces
				float yb0 = ys1;
				float yb1 = yb0 + blockUVSize.y;

				// View from negative x
				if(!Get(x - 1, y, z))
				{
					
					glm::vec3 normal(-1, 0, 0);					//add world position here instead of world matrix multiplication for every vertex every frame
					renderList.push_back(Vertex(glm::vec4(x,     y,     z,		blockType) + worldP, normal, glm::vec2(x0, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,     y,     z + 1,	blockType) + worldP, normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,     y + 1, z,		blockType) + worldP, normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,     y + 1, z,		blockType) + worldP, normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,     y,     z + 1,	blockType) + worldP, normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,     y + 1, z + 1,	blockType) + worldP, normal, glm::vec2(x1, ys0)));		
				} 
				// View from positive x
				if(!Get(x + 1, y, z))
				{
					glm::vec3 normal(1, 0, 0);
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z,		blockType) + worldP, normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z,		blockType) + worldP, normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z + 1,	blockType) + worldP, normal, glm::vec2(x0, ys1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z,		blockType) + worldP, normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType) + worldP, normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z + 1,	blockType) + worldP, normal, glm::vec2(x0, ys1)));
				}
				// View from negative y
				if(!Get(x, y-1, z))
				{
					glm::vec3 normal(0, -1, 0);
					renderList.push_back(Vertex(glm::vec4(x,	 y, z,			blockType) + worldP, normal, glm::vec2(x1, yb1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y, z,			blockType) + worldP, normal, glm::vec2(x0, yb1)));
					renderList.push_back(Vertex(glm::vec4(x,	 y, z + 1,		blockType) + worldP, normal, glm::vec2(x1, yb0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y, z,			blockType) + worldP, normal, glm::vec2(x0, yb1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y, z + 1,		blockType) + worldP, normal, glm::vec2(x0, yb0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y, z + 1,		blockType) + worldP, normal, glm::vec2(x1, yb0)));
				}
				// View from positive y
				if(!Get(x,y+1,z))
				{
					glm::vec3 normal(0, 1, 0);
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z + 1,	blockType) + worldP, normal, glm::vec2(x1, y0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType) + worldP, normal, glm::vec2(x0, y0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z,		blockType) + worldP, normal, glm::vec2(x1, y1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType) + worldP, normal, glm::vec2(x0, y0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z,		blockType) + worldP, normal, glm::vec2(x0, y1)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z,		blockType) + worldP, normal, glm::vec2(x1, y1)));
				}
				// View from negative z
				if(!Get(x, y, z - 1))
				{
					glm::vec3 normal(0, 0, -1);
					renderList.push_back(Vertex(glm::vec4(x,	 y,		z,		blockType) + worldP, normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z,		blockType) + worldP, normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z,		blockType) + worldP, normal, glm::vec2(x0, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z,		blockType) + worldP, normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z,		blockType) + worldP, normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z,		blockType) + worldP, normal, glm::vec2(x0, ys1)));
				}
				// View from positive z
				if(!Get(x, y, z + 1))
				{
					glm::vec3 normal(0, 0, 1);
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z + 1,	blockType) + worldP, normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType) + worldP, normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y,		z + 1,	blockType) + worldP, normal, glm::vec2(x0, ys1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType) + worldP, normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z + 1,	blockType) + worldP, normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y,		z + 1,	blockType) + worldP, normal, glm::vec2(x0, ys1)));
				}
			}
		}
	}

	numberOfElements = renderList.size();

	//Delete old mesh 
	if (chunkMesh)
		delete chunkMesh;

	//Don't create mesh if all blocks are air
	if (numberOfElements == 0)
		return;

	//Create vertexbuffer!
	chunkMesh = new Mesh();
	chunkMesh->CreateVertexBuffer(&renderList[0], numberOfElements);
	
}

void Chunk::Draw()
{
	if(changed)
		UpdateChunk();

	//Don't draw if there are no vertices of no mesh ( these go hand-in-hand and only one is actually needed, but keeping them both for security)
	if(!numberOfElements)
		return;

	if (!chunkMesh)
		return;

	chunkMesh->Draw();
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
