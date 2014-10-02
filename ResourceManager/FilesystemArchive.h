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
	/**
		Open the file and set the position indicator for reading to the beginning.
	*/
	bool Open();

	/**
		Close the file, no more work is going to be done with it until an Open is called.
	*/
	bool Close();

	/**
		Read byteCount number of bytes and store them in ptr. Increases the position indicator with byteCount.
	*/
	size_t Read(void* ptr, size_t byteCount);

	/**
		Set the position indicator to a specific location. The location is specified by an origin and an offset (both positive and negative) from the origin.
		See the File::Origin enum for information about the origin values.
	*/
	bool Seek(long int offset, File::Origin origin);

	/**
		Return the location of the position anchor, in bytes from the beginning of the file.
	*/
	long int Tell();

	/**
		Return the size of the file, in bytes.
	*/
	long int GetFileSize();
private:
	std::string filepath;
	FILE* file;
};