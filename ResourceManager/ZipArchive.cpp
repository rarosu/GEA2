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

		File* file = new ZipArchiveFile(archive, fileInfo, position);
		result.push_back(std::pair<std::string, File*>(filename, file));

		// Move to the next file in the archive.
		unzGoToNextFile(archive);
	}

	return result;
}



ZipArchiveFile::ZipArchiveFile(unzFile archive, const unz_file_info& info, const unz_file_pos& position)
{
	this->archive = archive;
	this->info = info;
	this->position = position;
}

ZipArchiveFile::~ZipArchiveFile()
{
	delete[] data;
}

bool ZipArchiveFile::Open()
{
	//unzLocateFile(archive  
 	if (unzGoToFilePos(archive, &position) != UNZ_OK)
	{
		return false;
	}

	if (unzOpenCurrentFile(archive) != UNZ_OK)
	{
		return false;
	}

	this->data = new char[this->info.uncompressed_size];

	unzReadCurrentFile(this->archive, this->data, this->info.uncompressed_size );

	return true;
}

CharBuffer ZipArchiveFile::GetStreamBuffer()
{
	return CharBuffer(this->data, this->data + this->info.uncompressed_size);
}
