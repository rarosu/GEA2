#include "Filesystem.h"
#include <iostream>

File* Filesystem::GetFile(const std::string& vpath)
{
	auto it = files.find(vpath);
	if (it == files.end())
	{
		std::cerr << "Unable to find file with virtual path: " << vpath << std::endl;
		return nullptr;
	}

	return it->second;
}