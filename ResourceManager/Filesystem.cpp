#include "Filesystem.h"
#include <iostream>
#include <cassert>

void CloseFile(File* file)
{
	bool good = file->Close();
	assert(good);
}

std::shared_ptr<File> Filesystem::GetFile(const std::string& vpath)
{
	auto it = files.find(vpath);

	if (it == files.end())
	{
		std::cerr << "Unable to find file with virtual path: " << vpath << std::endl;
		return nullptr;
	}

	if (!it->second->Open())
	{
		return nullptr;
	}

	return std::shared_ptr<File>(it->second, CloseFile);
}