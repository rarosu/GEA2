#pragma once

#include <vector>
#include <istream>

class CharBuffer : public std::streambuf
{
public:
	CharBuffer(char* begin, char* end)
	{
		setg(begin, begin, end); 
	}
};

class File
{
public:
	/**
		Specifies anchor points within the file, to set the cursor/position indicator to.
	*/
	enum Origin
	{
		ORIGIN_BEG,		// The beginning of the file.
		ORIGIN_CUR		// The current location of the position indicator.
	};

	virtual ~File() {}

	/**
		Open the file and set the position indicator for reading to the beginning.
	*/
	virtual bool Open() = 0;

	/**
		Close the file, no more work is going to be done with it until an Open is called.
	*/
	virtual bool Close() = 0;

	/**
		Read byteCount number of bytes and store them in ptr. Increases the position indicator with byteCount.
	*/
	virtual size_t Read(void* ptr, size_t byteCount) = 0;

	/**
		Set the position indicator to a specific location. The location is specified by an origin and an offset (both positive and negative) from the origin.
		See the File::Origin enum for information about the origin values.
	*/
	virtual bool Seek(long int offset, File::Origin origin) = 0;

	/**
		Return the location of the position anchor, in bytes from the beginning of the file.
	*/
	virtual long int Tell() = 0;

	/**
		Return the size of the file, in bytes.
	*/
	virtual long int GetFileSize() = 0;

	/**
		Get a streambuf object for reading the entire file as a text file.

		Usage:

			CharBuffer buffer = file.GetStreamBuffer();
			std::istream stream(&buffer);
			// Use stream to extract formatted text
	*/
	virtual CharBuffer GetStreamBuffer() = 0;


};

class Archive
{
public:
	virtual ~Archive() {}

	virtual bool Open(const std::string& path) = 0;
	virtual std::vector<std::pair<std::string, File*>> GetFiles() = 0;
};