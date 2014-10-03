#include "ChunkManager.h"


ChunkManager::ChunkManager(ID3D10Device* p_d3dDevice)
{
	m_d3dDevice = p_d3dDevice;

	memset(m_chunkList, 0, sizeof(m_chunkList));

	m_shader = new Shader();
	HR(m_shader->init(m_d3dDevice, "objectn.fx", 12 ));

	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCY; y++)
			for(int z = 0; z < SCZ; z++)
				m_chunkList[x][y][z] = new Chunk(m_d3dDevice, m_shader);

	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCY; y++)
			for(int z = 0; z < SCZ; z++) {
				if(x > 0)
					m_chunkList[x][y][z]->left = m_chunkList[x - 1][y][z];
				if(x < SCX - 1)
					m_chunkList[x][y][z]->right = m_chunkList[x + 1][y][z];
				if(y > 0)
					m_chunkList[x][y][z]->below = m_chunkList[x][y - 1][z];
				if(y < SCY - 1)
					m_chunkList[x][y][z]->above = m_chunkList[x][y + 1][z];
				if(z > 0)
					m_chunkList[x][y][z]->front = m_chunkList[x][y][z - 1];
				if(z < SCZ - 1)
					m_chunkList[x][y][z]->back = m_chunkList[x][y][z + 1];
			}

	m_nrOfChunks = SCX*SCX*SCZ;
	m_nrOfBlocks = m_nrOfChunks*CX*CY*CZ;
}


ChunkManager::~ChunkManager()
{
	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCX; y++)
			for(int z = 0; z < SCX; z++)
				delete m_chunkList[x][y][z];
}

void ChunkManager::render( D3DXMATRIX p_viewMatrix, D3DXMATRIX p_projMatrix )
{
	for(int x = 0; x < SCX; x++)
		for(int y = 0; y < SCY; y++)
			for(int z = 0; z < SCZ; z++)
				if(m_chunkList[x][y][z])
				{
					D3DXMATRIX worldM;
					D3DXMatrixTranslation(&worldM, x * CX, y * CY, z * CZ);
					m_chunkList[x][y][z]->render(worldM * p_viewMatrix * p_projMatrix);
				}
}

uint8_t ChunkManager::get( int x, int y, int z )
{
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;

	if(!m_chunkList[cx][cy][cz])
		return 0;
	else
		return m_chunkList[cx][cy][cz]->get(x, y, z);
}

void ChunkManager::set( int x, int y, int z, uint8_t type )
{
	int cx = x / CX;
	int cy = y / CY;
	int cz = z / CZ;

	x %= CX;
	y %= CY;
	z %= CZ;
	  
	if(!m_chunkList[cx][cy][cz])
	{
		m_nrOfChunks++;
		m_chunkList[cx][cy][cz] = new Chunk(m_d3dDevice, m_shader);
	}

	m_nrOfBlocks++;
	m_chunkList[cx][cy][cz]->set(x, y, z, type);
}

int ChunkManager::getNrOfBlocks()
{
	return m_nrOfBlocks;
}

int ChunkManager::getNrOfChunks()
{
	return m_nrOfChunks;
}

void ChunkManager::generateSimplexTerrain()
{
	for (int x = 0; x < SCX; x++)
	{
		for(int y = 0; y < SCY; y++)
		{
			for (int z = 0; z < SCZ; z++)
			{
				m_chunkList[x][y][z]->Noise(1, x, y, z);
			}
		}
	}
}
