#pragma once

#include "../Mesh.h"
#include <string>
#include <vector>
#include <cstdint>
#include <glm.hpp>
#include <mutex>


struct MetaWorldHeader;

//Number of textures in texture atlas in x and y coords. Change this to match actual texture atlas!
#define ATLASTEXTURES_X 16
#define ATLASTEXTURES_Y 16

typedef glm::detail::tvec4<uint8_t, glm::lowp> byte4;

class Chunk
{
public:
	Chunk(char* blockListMemory, const glm::vec3& worldPos, const MetaWorldHeader& metaHeader);
	~Chunk();

	//Get block type at chunk relative position
	uint8_t Get(int x, int y, int z);

	//Set block tpe at chunk relative position
	void Set(int x, int y, int z, uint8_t type);

	//Update the VB of the chunk if blocks have been altered
	void UpdateChunk(std::mutex* mutex);

	//Block list
	uint8_t* blockList;

	//Pointers to neighbour chunks, nullptr if no neighbour
	Chunk* left;
	Chunk* right;
	Chunk* below;
	Chunk* above;
	Chunk* front;
	Chunk* back;
	
	//Flag to tell the manager if the chunk need updating
	bool	changed;

	//Temp world matrix
	glm::mat4 worldMatrix;

	Buffer vertexBuffer;

	//Number of elements
	size_t numberOfElements;

	glm::vec4 worldP;

private:

	const MetaWorldHeader& metaWorldHeader;
	
	int GetBlockArrayIndex(int x, int y, int z);

	float x0[16];
	float x1[16];
	float y0[16];
	float y1[16];
	float ys0[16];
	float ys1[16];
	float yb0[16];
	float yb1[16];
};
