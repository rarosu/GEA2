#include <glm.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "../ResourceManager/RLE/rle.c"
#include <gtc/noise.hpp>

int SCX, SCY, SCZ, CX, CY, CZ, SEALEVEL;

int Export(const char* fileName);

int main(int argc, char* argv[])
{
	std::cout << "World Generator v1.0!\nEnter desired file name(no extension):" << std::endl;

	std::string filename;
	std::cin >> filename;
	
	std::cout << "Enter number of chunks: x y z" << std::endl;
	std::cin >> SCX >> SCY >> SCZ;

	std::cout << "Enter number of blocks per chunk: x y z" << std::endl;
	std::cin >> CX >> CY >> CZ;

	std::cout << "Enter water level(in blocks): h" << std::endl;
	std::cin >> SEALEVEL;

	int compressedSize = Export(filename.c_str());

	std::cout << "World name		: " << filename << std::endl;
	std::cout << "#Chunks			: " << SCX * SCY * SCZ << std::endl;
	std::cout << "#Chunks/block		: " << CX * CY * CZ << std::endl;
	std::cout << "#Total blocks		: " << CX * CY * CZ * SCX * SCY * SCZ << std::endl;
	std::cout << "Uncompressed size	: " << SCX * SCY * SCZ * 8 + 4 + CX * CY * CZ * SCX * SCY * SCZ << std::endl;
	std::cout << "Compressed size	: " << compressedSize << std::endl;

	std::cin.get();
}

void Set(int x, int y, int z, uint8_t type, unsigned char* blockData)
{
	if (x < 0 || x >= CX || y < 0 || y >= CY || z < 0 || z >= CZ)
		return;

	blockData[CZ * CY * x + CZ * y + z] = type;
}

uint8_t Get(int x, int y, int z, unsigned char* blockData)
{
	if (x < 0 || x >= CX || y < 0 || y >= CY || z < 0 || z >= CZ)
		return 0;

	return blockData[CZ * CY * x + CZ * y + z];
}

//2D Octave simplex noise
static float noise2d(float x, float y, int octaves, float persistence) {
	float sum = 0;
	float strength = 1.0;
	float scale = 1.0;

	for (int i = 0; i < octaves; i++)
	{
		sum += strength * glm::simplex<float>(glm::vec2(x, y)) * scale;
		scale *= 2.0;
		strength *= persistence;
	}

	return sum;
}

//3D octave simplex noise
static float noise3d_abs(float x, float y, float z, int octaves, float persistence) {
	float sum = 0;
	float strength = 1.0;
	float scale = 1.0;

	for (int i = 0; i < octaves; i++)
	{
		sum += strength * fabs(glm::simplex<float>(glm::vec3(x, y, z)) * scale);
		scale *= 2.0;
		strength *= persistence;
	}

	return sum;
}

//Chunk data generation
void Generate(int ax, int ay, int az, unsigned char* blockData)
{
	//Go through all blocks in X and Z to build each column from y == 0
	for (int x = 0; x < CX; x++)
	{
		for (int z = 0; z < CZ; z++)
		{
			// Land height, get the height at a specific X,Z coord
			float n = noise2d((x + ax * CX) / 256.0f, (z + az * CZ) / 256.0f, 5, 0.8f) * 2;
			int h = (int)(n * 2);
			int y = 0;

			// Land blocks, start building the Y column
			for (y = 0; y < CY; y++)
			{
				// Are we above "ground" level?
				if (y + ay * CY >= h)
				{
					// If we are not yet up to sea level, fill with water blocks
					if (y + ay * CY < SEALEVEL)
					{
						Set(x, y, z, 6, blockData);
						continue;
						// Otherwise, we are in the air
					}
					else
					{
						// A tree!
					/*	if (Get(x, y - 1, z) == 1 && (rand() % 200) == 0)
						{
							// Trunk
							h = (rand() & 0x3) + 3;
							for (int i = 0; i < h; i++)
								Set(x, y + i, z, 5);

							// Leaves
							for (int ix = -3; ix <= 3; ix++)
							{
								for (int iy = -3; iy <= 3; iy++)
								{
									for (int iz = -3; iz <= 3; iz++)
									{
										if (ix * ix + iy * iy + iz * iz < 8 + (rand() & 1) && !Get(x + ix, y + h + iy, z + iz))
											Set(x + ix, y + h + iy, z + iz, 4);
									}
								}
							}
						}*/
						break;
					}
				}

				// Random value used to determine land type
				float r = noise3d_abs((x + ax * CX) / 16.0f, (y + ay * CY) / 16.0f, (z + az * CZ) / 16.0f, 2, 1);
				r -= 0.5f; // Rescale r value to allow for more sand 
				// Sand layer
				if (n + r * 5 < 4)
					Set(x, y, z, 3, blockData);
				// Rock layer
				else if (n + r * 5 < 8)
					Set(x, y, z, 8, blockData);
				// Dirt layer, but use grass blocks for the top
				else if (r < 1.25)
					Set(x, y, z, (h < SEALEVEL || y + ay * CY < h - 1) ? 2 : 1, blockData);
				// Sometimes, ores. But only at least 10 below the grass line!
				else if (h < SEALEVEL || y + ay * CY < h - 10)
					Set(x, y, z, 7, blockData);
				else //Grass blocks as default, this is not correct but works OK
					Set(x, y, z, 1, blockData);
			}
		}
	}
}

int Export(const char* fileName)
{
	struct header_element
	{
		int adress;
		int size;
	};

	int numberOfBlocksPerChunk = CX * CY * CZ;
	int numberOfChunks = SCX * SCY * SCZ;
	int header_size = numberOfChunks * sizeof(header_element) + sizeof(int);
	int compressedSize = SCX * SCY * SCZ * 8 + 4; //Used as statistic, begin with header size. Return value.
	int percentChunk = numberOfChunks / 100; //1% of chunks

	header_element* header = new header_element[numberOfChunks];
	memset(header, 0, header_size - sizeof(int));
	std::ofstream s;
	s.open(fileName, std::ios_base::binary);

	s.write((char*)&header_size, sizeof(int));
	s.write((char*)header, header_size - sizeof(int));

	unsigned char* chunkData = new unsigned char[numberOfBlocksPerChunk];
	unsigned char* compressed = new unsigned char[numberOfBlocksPerChunk];

	int i = 0;
	for (int x = 0; x < SCX; ++x)
	{
		for (int y = 0; y < SCY; ++y)
		{
			for (int z = 0; z < SCZ; ++z)
			{
				memset(chunkData, 0, numberOfBlocksPerChunk);
				memset(compressed, 0, numberOfBlocksPerChunk);

				//Generate chunkdata into array for compression
				Generate(x, y, z, chunkData);

				
				header[i].size = RLE_Compress(chunkData, compressed, numberOfBlocksPerChunk);
				compressedSize += header[i].size;
				s.write((char*)compressed, header[i].size);

				int address = (i != 0) ? header[i - 1].adress + header[i - 1].size : header_size;
				header[i].adress = address;

				++i;
				
				if (i % percentChunk == 0)
					std::cout << "Progress: " << (int)(((float)i / (float)numberOfChunks) * 100.0f) << "%" << std::endl;
			}
		}
	}

	s.seekp(std::ios_base::beg + sizeof(int));
	s.write((char*)header, header_size - sizeof(int));

	s.flush();
	s.close();

	delete[] header;
	delete[] chunkData;
	delete[] compressed;

	return compressedSize;
}
