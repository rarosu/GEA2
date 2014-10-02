#pragma once

#define ZLIB_WINAPI

#include <map>
#include <string>
#include <unzip.h>
#include "Archive.h"

class ZipArchive : public Archive
{
public:
	~ZipArchive();

	bool Open(const std::string& path);
	std::vector<std::pair<std::string, File*>> GetFiles();
private:
	unzFile archive;
	std::map<std::string, size_t> offsets;
	std::string archivePath;
};

class ZipArchiveFile : public File
{
public:
	ZipArchiveFile(unzFile archive, const unz_file_info& info, const unz_file_pos& position);
	~ZipArchiveFile();
	bool Open();
	bool Close();
	size_t Read(void* ptr, size_t byteCount);
	bool Seek(long int offset, File::Origin origin);
	long int Tell();
	long int GetFileSize();
private:
	unzFile archive;
	unz_file_info info;
	unz_file_pos position;
};