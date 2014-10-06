#pragma once

#ifdef ALL_PUBLIC
	#define PUBLIC public
	#define PRIVATE public
	#define PROTECTED public
#else
	#define PUBLIC public
	#define PRIVATE private
	#define PROTECTED protected
#endif

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include "Archive.h"

class Filesystem
{
PUBLIC:
	template <typename T>
	bool AddArchive(const std::string& path);

	std::shared_ptr<File> GetFile(const std::string& vpath);
PRIVATE:
	std::vector<Archive*> archives;
	std::map<std::string, File*> files;
	std::mutex mutex;
};


template <typename T>
bool Filesystem::AddArchive(const std::string& path)
{
	T* archive = new T;
	if (!archive->Open(path))
	{
		delete archive;
		return false;
	}

	this->archives.push_back(archive);

	std::vector<std::pair<std::string, File*>> archiveFiles = archive->GetFiles();
	for (auto pair : archiveFiles)
	{
		if (this->files.find(pair.first) != this->files.end())
		{
			std::cerr << "Conflicting files: " << pair.first << std::endl;
			continue;
		}

		this->files.insert(pair);
	}

	return true;
}