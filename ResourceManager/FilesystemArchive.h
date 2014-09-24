#pragma once

#include "Archive.h"

class FilesystemArchive : public Archive
{
public:

private:

};

class FilesystemFile : public File
{
public:
private:
	FILE* file;
};