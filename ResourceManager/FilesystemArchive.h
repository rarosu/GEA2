#pragma once

#include "Archive.h"

class FilesystemArchive : public Archive
{
public:
	~FilesystemArchive();
	
	/**
		Clears the archive and stores the path. The path can be either absolute or relative. This will not search the directory.
	*/
	bool Open(const std::string& path);

	/**
		Searches the directory, specified by the archive path, recursively.
	*/
	std::vector<std::pair<std::string, File*>> GetFiles();
private:
	std::string path;

	/** 
		Search one directory recursively.
	*/
	void SearchDirectory(const std::string& directory, const std::string& relative, std::vector<std::pair<std::string, File*>>& files);
};

class FilesystemFile : public File
{
public:
	FilesystemFile(const std::string& filepath);
	~FilesystemFile();

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