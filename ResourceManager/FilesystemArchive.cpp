#define WIN32_LEAN_AND_MEAN

#include "FilesystemArchive.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <iostream>
#include <filesystem>

FilesystemArchive::~FilesystemArchive()
{

}

bool FilesystemArchive::Open(const std::string& path)
{
	this->path = path;

	/*
	for (auto file : files)
	{
		file.second->Close();
		delete file.second;
	}
	files.clear();

	SearchDirectory(path, "");
	*/

	return true;
}



std::vector<std::pair<std::string, File*>> FilesystemArchive::GetFiles()
{
	std::vector<std::pair<std::string, File*>> files;
	SearchDirectory(path, "", files);
	return files;
}


void FilesystemArchive::SearchDirectory(const std::string& directory, const std::string& relative, std::vector<std::pair<std::string, File*>>& files)
{
	for (std::tr2::sys::directory_iterator it(directory), end; it != end; ++it)
	{
		if (std::tr2::sys::is_directory(it->path()))
		{
			SearchDirectory(it->path(), relative + it->path().filename() + std::tr2::sys::slash<std::tr2::sys::path>::value, files);
		}

		if (std::tr2::sys::is_regular(it->path()))
		{
			files.push_back(std::pair<std::string, File*>(relative + it->path().filename(), new FilesystemFile(it->path().relative_path())));

			/*
			std::cout << "Found regular: " << it->path().filename() << std::endl;
			std::cout << "\tParent path: " << it->path().parent_path() << std::endl;
			std::cout << "\tRelative path: " << it->path().relative_path() << std::endl;
			std::cout << "\tRoot path: " << it->path().root_path() << std::endl;
			std::cout << "\tBranch path: " << it->path().branch_path() << std::endl;
			std::cout << "\tStem: " << it->path().stem() << std::endl;
			std::cout << "\tMyRelative: " << relative + it->path().filename() << std::endl;
			//std::cout << "Absolute path relative directory: " << std::tr2::sys:://std::tr2::sys::absolute(
			*/
		}
	}
}



FilesystemFile::FilesystemFile(const std::string& filepath) 
	: file(nullptr)
{
	this->filepath = filepath;
}

FilesystemFile::~FilesystemFile()
{
	if (file != nullptr)
		fclose(file);
}

bool FilesystemFile::Open()
{
	// Make sure the file cannot be opened if it is already opened.
	if (file != nullptr)
		return false;

	file = fopen(filepath.c_str(), "rb");
	return file != nullptr;
}

bool FilesystemFile::Close()
{
	bool result = fclose(file) != EOF;
	file = nullptr;
	return result;
}

size_t FilesystemFile::Read(void* ptr, size_t byteCount)
{
	std::lock_guard<std::mutex> lock(mutex);
	return fread(ptr, 1, byteCount, file);
}

bool FilesystemFile::Seek(long int offset, File::Origin origin)
{
	int o;
	switch (origin)
	{
		case File::ORIGIN_BEG:
			o = SEEK_SET;
			break;
		case File::ORIGIN_CUR:
			o = SEEK_CUR;
			break;
		default:
			o = SEEK_CUR;
	}

	std::lock_guard<std::mutex> lock(mutex);
	return fseek(file, offset, o) != 0;
}

long int FilesystemFile::Tell()
{
	std::lock_guard<std::mutex> lock(mutex);
	return ftell(file);
}

long int FilesystemFile::GetFileSize()
{
	std::lock_guard<std::mutex> lock(mutex);
	long int current = ftell(file);
	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fseek(file, current, SEEK_SET);
	return size;
}
