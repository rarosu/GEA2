#include "Chunk.h"
#include <iostream>
#include <gtc/noise.hpp>
#include <gtc/matrix_transform.hpp>

Chunk::Chunk(const glm::vec3& worldPos)
	: changed(true), numberOfElements(0), left(nullptr), right(nullptr), below(nullptr), above(nullptr), front(nullptr), back(nullptr), chunkMesh(nullptr)
{
	worldMatrix = glm::translate(worldMatrix, worldPos);
	memset(blockList, 0, sizeof(blockList));
}

Chunk::~Chunk()
{
	delete chunkMesh;
}

void Chunk::UpdateChunk()
{
	changed = false;
	float testUV = 1.0f / 512.0f;

	std::vector<Vertex> renderList;

	bool vis = false;

	for(uint8_t x = 0; x < CX; x++) 
	{
		for(uint8_t y = 0; y < CY; y++) 
		{
			for(uint8_t z = 0; z < CZ; z++) 
			{
				// Empty block?
				if(!blockList[x][y][z])
				{
					vis = false;
					continue;
				}
				
				///Calculate correct UV mapped to atlas
				uint8_t blockType = blockList[x][y][z];

				glm::vec2 blockUVSize = glm::vec2(1.0f / ATLASTEXTURES_X, 1.0f / ATLASTEXTURES_Y);
				
				glm::ivec2 textureAtlasPosition = glm::ivec2((blockType - 1) % ATLASTEXTURES_X, (blockType - 1) / ATLASTEXTURES_Y);

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
					
					glm::vec3 normal(-1, 0, 0);
					renderList.push_back(Vertex(glm::vec4(x,     y,     z,		blockType), normal, glm::vec2(x0, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,     y,     z + 1,	blockType), normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,     y + 1, z,		blockType), normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,     y + 1, z,		blockType), normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,     y,     z + 1,	blockType), normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,     y + 1, z + 1,	blockType), normal, glm::vec2(x1, ys0)));		
				} 
				// View from positive x
				if(!Get(x + 1, y, z))
				{
					glm::vec3 normal(1, 0, 0);
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z,		blockType), normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z,		blockType), normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z + 1,	blockType), normal, glm::vec2(x0, ys1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z,		blockType), normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType), normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z + 1,	blockType), normal, glm::vec2(x0, ys1)));
				}
				// View from negative y
				if(!Get(x, y-1, z))
				{
					glm::vec3 normal(0, -1, 0);
					renderList.push_back(Vertex(glm::vec4(x,	 y, z,			blockType), normal, glm::vec2(x1, yb1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y, z,			blockType), normal, glm::vec2(x0, yb1)));
					renderList.push_back(Vertex(glm::vec4(x,	 y, z + 1,		blockType), normal, glm::vec2(x1, yb0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y, z,			blockType), normal, glm::vec2(x0, yb1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y, z + 1,		blockType), normal, glm::vec2(x0, yb0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y, z + 1,		blockType), normal, glm::vec2(x1, yb0)));
				}
				// View from positive y
				if(!Get(x,y+1,z))
				{
					glm::vec3 normal(0, 1, 0);
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z + 1,	blockType), normal, glm::vec2(x1, y0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType), normal, glm::vec2(x0, y0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z,		blockType), normal, glm::vec2(x1, y1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType), normal, glm::vec2(x0, y0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z,		blockType), normal, glm::vec2(x0, y1)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z,		blockType), normal, glm::vec2(x1, y1)));
				}
				// View from negative z
				if(!Get(x, y, z - 1))
				{
					glm::vec3 normal(0, 0, -1);
					renderList.push_back(Vertex(glm::vec4(x,	 y,		z,		blockType), normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z,		blockType), normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z,		blockType), normal, glm::vec2(x0, ys1)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z,		blockType), normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z,		blockType), normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z,		blockType), normal, glm::vec2(x0, ys1)));
				}
				// View from positive z
				if(!Get(x, y, z + 1))
				{
					glm::vec3 normal(0, 0, 1);
					renderList.push_back(Vertex(glm::vec4(x + 1, y,		z + 1,	blockType), normal, glm::vec2(x1, ys1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType), normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y,		z + 1,	blockType), normal, glm::vec2(x0, ys1)));
					renderList.push_back(Vertex(glm::vec4(x + 1, y + 1, z + 1,	blockType), normal, glm::vec2(x1, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y + 1, z + 1,	blockType), normal, glm::vec2(x0, ys0)));
					renderList.push_back(Vertex(glm::vec4(x,	 y,		z + 1,	blockType), normal, glm::vec2(x0, ys1)));
				}
			}
		}
	}

	numberOfElements = renderList.size();

	if (chunkMesh)
		delete chunkMesh;

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

	if(!numberOfElements)
		return;
	if (!chunkMesh)
		return;

	chunkMesh->Draw();
}


void Chunk::Set( int x, int y, int z, uint8_t type )
{
	//If coordinates are outside this chunk, find the right one.
	if(x < 0) 
	{
		if(left)
			left->Set(x + CX, y, z, type);
		return;
	}
	if(x >= CX) 
	{
		if(right)
			right->Set(x - CX, y, z, type);
		return;
	}
	if(y < 0) 
	{
		if(below)
			below->Set(x, y + CY, z, type);
		return;
	}
	if(y >= CY) 
	{
		if(above)
			above->Set(x, y - CY, z, type);
		return;
	}
	if(z < 0) 
	{
		if(front)
			front->Set(x, y, z + CZ, type);
		return;
	}
	if(z >= CZ) 
	{
		if(back)
			back->Set(x, y, z - CZ, type);
		return;
	}

	// Change the block
	blockList[x][y][z] = type;
	changed = true;

	// When updating blocks at the edge of this chunk,
	// visibility of blocks in the neighbouring chunk might change.
	if(x == 0 && left)
		left->changed = true;
	if(x == CX - 1 && right)
		right->changed = true;
	if(y == 0 && below)
		below->changed = true;
	if(y == CY - 1 && above)
		above->changed = true;
	if(z == 0 && front)
		front->changed = true;
	if(z == CZ - 1 && back)
		back->changed = true;
}

uint8_t Chunk::Get( int x, int y, int z )
{
	if(x < 0)
		return left ? left->blockList[x + CX][y][z] : 0;
	if(x >= CX)
		return right ? right->blockList[x - CX][y][z] : 0;
	if(y < 0)
		return below ? below->blockList[x][y + CY][z] : 0;
	if(y >= CY)
		return above ? above->blockList[x][y - CY][z] : 0;
	if(z < 0)
		return front ? front->blockList[x][y][z + CZ] : 0;
	if(z >= CZ)
		return back ? back->blockList[x][y][z - CZ] : 0;

	return blockList[x][y][z];
}

static float noise2d(float x, float y, int seed, int octaves, float persistence) {
	float sum = 0;
	float strength = 1.0;
	float scale = 1.0;

	for(int i = 0; i < octaves; i++) 
	{
		sum += strength * glm::simplex<float>(glm::vec2(x, y)) * scale;
		scale *= 2.0;
		strength *= persistence;
	}

	return sum;
}

static float noise3d_abs(float x, float y, float z, int seed, int octaves, float persistence) {
	float sum = 0;
	float strength = 1.0;
	float scale = 1.0;

	for(int i = 0; i < octaves; i++) 
	{
		sum += strength * fabs(glm::simplex<float>(glm::vec3(x, y, z)) * scale);
		scale *= 2.0;
		strength *= persistence;
	}

	return sum;
}

void Chunk::Noise(int seed, int ax, int ay, int az) 
{
	for(int x = 0; x < CX; x++) 
	{
		for(int z = 0; z < CZ; z++) 
		{
			// Land height
			float n = noise2d((x + ax * CX) / 256.0f, (z + az * CZ) / 256.0f, seed, 5, 0.8f) * 2;
			int h = (int)(n * 2);
			int y = 0;

			// Land blocks
			for(y = 0; y < CY; y++) 
			{
				// Are we above "ground" level?
				if(y + ay * CY >= h) 
				{
					// If we are not yet up to sea level, fill with water blocks
					if(y + ay * CY < SEALEVEL) 
					{
						Set(x, y, z, 6);
						continue;
						// Otherwise, we are in the air
					} 
					else 
					{
						// A tree!
						if(Get(x, y - 1, z) == 8 && (rand() & 0xff) == 0) 
						{
							// Trunk
							h = (rand() & 0x3) + 3;
							for(int i = 0; i < h; i++)
								Set(x, y + i, z, 5);

							// Leaves
							for(int ix = -3; ix <= 3; ix++) 
							{ 
								for(int iy = -3; iy <= 3; iy++) 
								{ 
									for(int iz = -3; iz <= 3; iz++) 
									{ 
										if(ix * ix + iy * iy + iz * iz < 8 + (rand() & 1) && !Get(x + ix, y + h + iy, z + iz))
											Set(x + ix, y + h + iy, z + iz,  4);
									}
								}
							}
						}
						break;
					}
				}

				// Random value used to determine land type
				float r = noise3d_abs((x + ax * CX) / 16.0f, (y + ay * CY) / 16.0f, (z + az * CZ) / 16.0f, -seed, 2, 1);
				//r--; // Rescale r value to allow for more sand 
				// Sand layer
				if(n + r * 5 < 4)
					Set(x, y, z, 3);
				// Rock layer
				else if(n + r * 5 < 8)
					Set(x, y, z, 8);
				// Dirt layer, but use grass blocks for the top
				else if(r < 1.25)
					Set(x, y, z, (h < SEALEVEL || y + ay * CY < h - 1) ? 2 : 1);
				// Sometimes, ores. But only 3 beloy the grass line!
				else if (h < SEALEVEL || y + ay * CY < h - 10)
					Set(x, y, z, 7);
				else 
					Set(x, y, z, 1);
			}
		}
	}
}