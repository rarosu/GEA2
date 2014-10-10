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
	std::cout << "World Generator v1.0!" << std::endl;
	std::string filename;
	while (1)
	{
		std::cout << "Enter desired file name(no extension):" << std::endl;
		std::cin >> filename;

		//Error checking
		if (std::cin.fail() || filename.length() == 0)
		{
			std::cin.clear();
			std::cin.ignore();
			std::cout << "WROOOONG!" << std::endl;
			continue;
		}

		break;
	}

	while (1)
	{
		std::cout << "Enter number of chunks: x y z" << std::endl;
		std::cin >> SCX >> SCY >> SCZ;

		//Error checking
		if (std::cin.fail() || !SCX || !SCY || !SCZ)
		{
			std::cin.clear();
			std::cin.ignore();
			std::cout << "WROOOONG!" << std::endl;
			continue;
		}

		break;
	}

	while (1)
	{
		std::cout << "Enter number of blocks per chunk: x y z" << std::endl;
		std::cin >> CX >> CY >> CZ;

		//Error checking
		if (std::cin.fail() || !CX || !CY || !CZ)
		{
			std::cin.clear();
			std::cin.ignore();
			std::cout << "WROOOONG!" << std::endl;
			continue;
		}
	
		break;
	}

	while (1)
	{
		std::cout << "Enter water level(in blocks): h" << std::endl;
		std::cin >> SEALEVEL;

		//Error checking
		if (std::cin.fail() || SEALEVEL < 0 || SEALEVEL >= CY)
		{
			std::cin.clear();
			std::cin.ignore();
			std::cout << "WROOOONG!" << std::endl;
			continue;
		}

		break;
	}

	int compressedSize = Export(filename.c_str());

	std::cout << "World name        : " << filename << std::endl;
	std::cout << "#Chunks           : " << SCX * SCY * SCZ << std::endl;
	std::cout << "#Chunks/block     : " << CX * CY * CZ << std::endl;
	std::cout << "#Total blocks     : " << CX * CY * CZ * SCX * SCY * SCZ << std::endl;
	std::cout << "Uncompressed size : " << SCX * SCY * SCZ * 8 + 4 + CX * CY * CZ * SCX * SCY * SCZ << std::endl;
	std::cout << "Compressed size   : " << compressedSize << std::endl;
	std::cout << "Size ratio        : " << (float)compressedSize / (float)(SCX * SCY * SCZ * 8 + 4 + CX * CY * CZ * SCX * SCY * SCZ) << std::endl;
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
		sum += strength * glm::simplex<float>(glm::vec2(x, y)) * scale + 8;
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
			float n = noise2d((x + ax * CX) / 256.0f, (z + az * CZ) / 256.0f, 5, 0.8f) - 26;
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
						if (Get(x, y - 1, z, blockData) == 1 && (rand() % 200) == 0)
						{
							// Trunk
							h = (rand() & 0x3) + 3;
							for (int i = 0; i < h; i++)
								Set(x, y + i, z, 5, blockData);

							// Leaves
							for (int ix = -3; ix <= 3; ix++)
							{
								for (int iy = -3; iy <= 3; iy++)
								{
									for (int iz = -3; iz <= 3; iz++)
									{
										if (ix * ix + iy * iy + iz * iz < 8 + (rand() & 1) && !Get(x + ix, y + h + iy, z + iz, blockData))
											Set(x + ix, y + h + iy, z + iz, 4, blockData);
									}
								}
							}
						}
						break;
					}
				}
				if (y + ay * CY < 11)
				{
					Set(x, y, z, 3, blockData);
					continue;
				}
				// Random value used to determine land type
				float r = noise3d_abs((x + ax * CX) / 16.0f, (y + ay * CY) / 16.0f, (z + az * CZ) / 16.0f, 2, 1);
				r -= 1.0f; // Rescale r value to allow for more sand 
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

	struct header_global
	{
		int header_size;
		int SCX;
		int SCY;
		int SCZ;
		int CX;
		int CY;
		int CZ;
	};

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
	
	int numberOfBlocksPerChunk = CX * CY * CZ;
	int numberOfChunks = SCX * SCY * SCZ;

	//Set up the global header
	header_global global_header;
	global_header.header_size = numberOfChunks * sizeof(header_element) + sizeof(header_global);
	global_header.SCX = SCX;
	global_header.SCY = SCY;
	global_header.SCZ = SCZ;
	global_header.CX = CX;
	global_header.CY = CY;
	global_header.CZ = CZ;

	int compressedSize = global_header.header_size; //Log compressed size in loop below as stats. begin with header size. Return this value from method.

	int percentChunk = numberOfChunks / 100; //1% of chunks

	//Create header element array and set to 0
	header_element* header = new header_element[numberOfChunks];
	memset(header, 0, global_header.header_size - sizeof(header_global));

	//Set up out file stream
	std::ofstream s;
	std::string assetWorldPath = "../Assets/Worlds/";
	assetWorldPath.append(fileName);
	assetWorldPath.append(".world");

	s.open(assetWorldPath, std::ios_base::binary);

	//Write the complete header_global
	s.write((char*)&global_header, sizeof(header_global));
	//Write empty header element data to reserve this space
	s.write((char*)header, global_header.header_size - sizeof(header_global));

	//In and out data for RLE compression
	unsigned char* chunkData = new unsigned char[numberOfBlocksPerChunk];
	unsigned char* compressed = new unsigned char[numberOfBlocksPerChunk];

	int i = 0;
	for (int x = 0; x < SCX; ++x)
	{
		for (int y = 0; y < SCY; ++y)
		{
			for (int z = 0; z < SCZ; ++z)
			{
				//Reset all blocks in chunk to 0 (=air)
				memset(chunkData, 0, numberOfBlocksPerChunk);

				//Generate chunkdata into array for compression
				Generate(x, y, z, chunkData);

				//Compress!
				header[i].size = RLE_Compress(chunkData, compressed, numberOfBlocksPerChunk);
				
				//Log compressed size for stats
				compressedSize += header[i].size;
				
				//Write compressed chunk to file
				s.write((char*)compressed, header[i].size);
				
				//Calculate the address of this chunk!
				//If first chunk, get "previous" address as the end of header data, that't where the chunk data begins!
				int address = (i != 0) ? header[i - 1].adress + header[i - 1].size : global_header.header_size;
				header[i].adress = address;

				//Counter to walk in the header array.
				++i;
				
				//Percent calc and print
				if (i % percentChunk == 0)
					std::cout << "Progress: " << (int)(((float)i / (float)numberOfChunks) * 100.0f) + 1 << "%" << std::endl;
			}
		}
	}

	//Seek to the end of the global header to write all the header elements
	s.seekp(std::ios_base::beg + sizeof(header_global));
	//Write header element data
	s.write((char*)header, global_header.header_size - sizeof(header_global));

	s.flush();
	s.close();

	delete[] header;
	delete[] chunkData;
	delete[] compressed;

	return compressedSize;
}
