#pragma once

#include "Archive.h"

class FilesystemArchive : public Archive
{
public:
	/**
		Clears the archive and searches a directory recursively, adding all files it finds.

		path should be the relative or absolute path to a directory.
	*/
	bool Open(const std::string& path);
	std::vector<std::pair<std::string, File*>> GetFiles();
private:
	std::vector<std::pair<std::string, File*>> files;

	/** 
		Search one directory recursively.
	*/
	void SearchDirectory(const std::string& directory);
};

class FilesystemFile : public File
{
public:
private:
	FILE* file;
};