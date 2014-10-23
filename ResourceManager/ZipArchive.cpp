#include "ZipArchive.h"
#include <iostream>

bool ZipArchive::Open(const std::string& path)
{
	archivePath = path;

	archive = unzOpen(path.c_str());
	if (archive == nullptr)
	{
		return false;
	}

	return true;
}

ZipArchive::~ZipArchive()
{
	if (archive != nullptr)
		unzClose(archive);
}

std::vector<std::pair<std::string, File*>> ZipArchive::GetFiles()
{
	std::vector<std::pair<std::string, File*>> result;

	unz_global_info globalInfo;
	if (unzGetGlobalInfo(archive, &globalInfo) != UNZ_OK)
	{
		return result;
	}

	for (uLong i = 0; i < globalInfo.number_entry; ++i)
	{
		// Get the filename size.
		unz_file_info fileInfo;
		if (unzGetCurrentFileInfo(archive, &fileInfo, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
		{
			std::cerr << "Failed to read file info. Entry " << i << " in " << archivePath << std::endl;
			unzGoToNextFile(archive);
			continue;
		}

		// Get the filename now that we have the filename size.
		std::string filename;
		filename.resize(fileInfo.size_filename);

		if (unzGetCurrentFileInfo(archive, &fileInfo, &filename[0], filename.size(), NULL, 0, NULL, 0) != UNZ_OK)
		{
			std::cerr << "Failed to read file info. Entry " << i << " in " << archivePath << std::endl;
			unzGoToNextFile(archive);
			continue;
		}

		// Do not add directories.
		if (filename[filename.size() - 1] == '/')
		{
			unzGoToNextFile(archive);
			continue;
		}

		// Get the position of the file and create a file.
		unz_file_pos position;
		if (unzGetFilePos(archive, &position) != UNZ_OK)
		{
			std::cerr << "Failed to read file info. Entry " << i << " in " << archivePath << std::endl;
			continue;
		}

		File* file = new ZipArchiveFile(archive, fileInfo, position, &mutex);
		result.push_back(std::pair<std::string, File*>(filename, file));

		// Move to the next file in the archive.
		unzGoToNextFile(archive);
	}

	return result;
}



ZipArchiveFile::ZipArchiveFile(unzFile archive, const unz_file_info& info, const unz_file_pos& position, std::mutex* mutex)
{
	this->archive = archive;
	this->info = info;
	this->position = position;
	this->mutex = mutex;
}

ZipArchiveFile::~ZipArchiveFile()
{
}

bool ZipArchiveFile::Open()
{
	this->mutex->lock();

 	if (unzGoToFilePos(archive, &position) != UNZ_OK)
	{
		std::cerr << "ZipArchive: Failed to go to file position" << std::endl;
		this->mutex->unlock();
		return false;
	}

	if (unzOpenCurrentFile(archive) != UNZ_OK)
	{
		std::cerr << "ZipArchive: Failed to open current file" << std::endl;
		this->mutex->unlock();
		return false;
	}

	return true;
}

bool ZipArchiveFile::Close()
{
	if (unzCloseCurrentFile(archive) != UNZ_OK)
	{
		std::cerr << "ZipArchive: Failed to close current file" << std::endl;
		this->mutex->unlock();
		return false;
	}
	this->mutex->unlock();
	return true;
}

size_t ZipArchiveFile::Read(void* ptr, size_t byteCount)
{
	return unzReadCurrentFile(archive, ptr, byteCount);
}

bool ZipArchiveFile::Seek(long int offset, File::Origin origin)
{
	std::cerr << "Seek is not supported for ZipArchive" << std::endl;
	return false;
}

long int ZipArchiveFile::Tell()
{
	std::cerr << "Tell is not supported for ZipArchive" << std::endl;
	return 0;
}

long int ZipArchiveFile::GetFileSize()
{
	return info.uncompressed_size;
}