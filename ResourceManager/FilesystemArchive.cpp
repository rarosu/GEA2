#define WIN32_LEAN_AND_MEAN

#include "FilesystemArchive.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <iostream>

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
	// Traverse the directory at the given path recursively.
	TCHAR pathPattern[MAX_PATH];
	WIN32_FIND_DATA fileFindData;
	HANDLE file;

	PathCombine(pathPattern, directory.c_str(), "*");
	file = FindFirstFile(pathPattern, &fileFindData);
	while (file != INVALID_HANDLE_VALUE)
	{
		PathCombine(pathPattern, directory.c_str(), fileFindData.cFileName);
		if (fileFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			FilesystemArchive::Open(pathPattern);
		}
		else
		{
			std::cout << pathPattern << std::endl;
		}
	}
}