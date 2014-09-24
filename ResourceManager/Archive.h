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
	virtual ~File() {}
	virtual bool Open() = 0;
	virtual CharBuffer GetStreamBuffer() = 0;
};

class Archive
{
public:
	virtual ~Archive() {}

	virtual bool Open(const std::string& path) = 0;
	virtual std::vector<std::pair<std::string, File*>> GetFiles() = 0;
};