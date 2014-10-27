#pragma once

const int CHUNK_LOAD_THREADS = 16;
const int DEFAULT_CHUNK_LOAD_DISTANCE = 16;
const int MAX_CHUNK_LOAD_DISTANCE = 25;
const int MAX_CHUNKS_IN_MEM = MAX_CHUNK_LOAD_DISTANCE * 2 * MAX_CHUNK_LOAD_DISTANCE * 4; //times 4 because load distance is the radius and we need to load 2 times the radius in x and z
