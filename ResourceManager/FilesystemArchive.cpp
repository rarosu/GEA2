#define WIN32_LEAN_AND_MEAN

#include "FilesystemArchive.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <iostream>
#include <filesystem>

bool FilesystemArchive::Open(const std::string& path)
{
	for (auto file : files)
	{
		file.second->Close();
	}

	files.clear();

	SearchDirectory(path);

	return true;
}



std::vector<std::pair<std::string, File*>> FilesystemArchive::GetFiles()
{
	return files;
}


void FilesystemArchive::SearchDirectory(const std::string& directory)
{
	for (std::tr2::sys::directory_iterator it(directory), end; it != end; ++it)
	{
		if (std::tr2::sys::is_directory(it->path()))
		{
			SearchDirectory(it->path());
		}

		if (std::tr2::sys::is_regular(it->path()))
		{
			std::cout << "Found regular: " << it->path().filename() << std::endl;
			std::cout << "\tParent path: " << it->path().parent_path() << std::endl;
			std::cout << "\tRelative path: " << it->path().relative_path() << std::endl;
			std::cout << "\tRoot path: " << it->path().root_path() << std::endl;
			std::cout << "\tBranch path: " << it->path().branch_path() << std::endl;
			std::cout << "\tStem: " << it->path().stem() << std::endl;
			//std::cout << "Absolute path relative directory: " << std::tr2::sys:://std::tr2::sys::absolute(
		}
	}
}



bool FilesystemFile::Open()
{
	file = fopen(filepath.c_str(), "rb");
	return file != NULL;
}

bool FilesystemFile::Close()
{
	return fclose(file) != EOF;
}

size_t FilesystemFile::Read(void* ptr, size_t byteCount)
{
	return fread(ptr, byteCount, 1, file);
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

	return fseek(file, offset, o) != 0;
}

long int FilesystemFile::Tell()
{
	return ftell(file);
}

long int FilesystemFile::GetFileSize()
{
	long int current = ftell(file);
	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fseek(file, current, SEEK_SET);
	return size;
}
