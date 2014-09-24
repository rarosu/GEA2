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

#include <map>
#include <string>
#include <functional>
#include <vector>
#include <iostream>
#include "Archive.h"

// Forward declare
class ResourceBase;

template <typename T>
class Resource;

class ResourceManager;



class ResourceBase
{
	template <typename T>
	friend class Resource;
PUBLIC:
	ResourceBase();
	virtual ~ResourceBase();
PRIVATE:
	int refCount;
};



template <typename T>
class Resource
{
PUBLIC:
	Resource();
	Resource(T* resource);
	Resource(const Resource<T>& resource);
	~Resource();

	T* operator->();
	T& operator*();
	Resource<T>& operator=(T* resource);
	Resource<T>& operator=(const Resource<T>& resource);

	bool operator==(const T* resource) const;
	bool operator==(const Resource<T>& resource) const;
	bool operator!=(const T* resource) const;
	bool operator!=(const Resource<T>& resource) const;
PRIVATE:
	ResourceBase* resource;
};


class ResourceManager
{
PUBLIC:
	template <typename T>
	bool AddArchive(const std::string& path);

	template <typename T>
	Resource<T> Load(const std::string& path, std::function<void()>& processor);

	template <typename T>
	Resource<T> Create(const std::string& name);
PRIVATE:
	// Map a hash to a resource.
	std::map<size_t, ResourceBase*> resources;
	std::hash<std::string> stringHasher;
	std::hash<size_t> intHasher;
	
	std::vector<Archive*> archives;
	std::map<std::string, File*> files;

	//std::vector<Archive> archives;
	//std::map<std::string, Archive> archives;
};



#pragma region ResourceBase

ResourceBase::ResourceBase()
	: refCount(0) {}

ResourceBase::~ResourceBase() {}


#pragma endregion


#pragma region Resource

template <typename T>
Resource<T>::Resource()
{
	resource = nullptr;
}

template <typename T>
Resource<T>::Resource(T* resource)
{
	this->resource = static_cast<ResourceBase*>(resource);
	this->resource->refCount++;
}

template <typename T>
Resource<T>::Resource(const Resource<T>& resource)
{
	this->resource = resource.resource;
	this->resource->refCount++;
}

template <typename T>
Resource<T>::~Resource()
{
	if (this->resource != nullptr)
		this->resource->refCount--;
}

template <typename T>
T* Resource<T>::operator->()
{
	return static_cast<T*>(this->resource);
}

template <typename T>
T& Resource<T>::operator*()
{
	return *static_cast<T*>(this->resource);
}

template <typename T>
Resource<T>& Resource<T>::operator=(T* resource)
{
	if (this->resource == resource)
		return (*this);

	if (this->resource != nullptr)
		this->resource->refCount--;

	this->resource = static_cast<ResourceBase*>(resource);
	this->resource->refCount++;

	return (*this);
}

template <typename T>
Resource<T>& Resource<T>::operator=(const Resource<T>& resource)
{
	if (this->resource == resource.resource)
		return (*this);

	if (this->resource != nullptr)
		this->resource->refCount--;

	this->resource = resource.resource;
	this->resource->refCount++;

	return (*this);
}

template <typename T>
bool Resource<T>::operator==(const T* resource) const
{
	return this->resource == resource;
}

template <typename T>
bool Resource<T>::operator==(const Resource<T>& resource) const
{
	return this->resource == resource.resource;
}

template <typename T>
bool Resource<T>::operator!=(const T* resource) const
{
	return !((*this) == resource);
}

template <typename T>
bool Resource<T>::operator!=(const Resource<T>& resource) const
{
	return !((*this) == resource);
}


#pragma endregion


#pragma region ResourceManager

template <typename T>
bool ResourceManager::AddArchive(const std::string& path)
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

template <typename T>
Resource<T> ResourceManager::Load(const std::string& path, std::function<void()>& processor)
{
	return Resource<T>();
}

template <typename T>
Resource<T> ResourceManager::Create(const std::string& name)
{
	T* resource = new T;
	size_t hash = stringHasher(name);

	resources[hash] = resource;
	return Resource<T>(resource);
}

#pragma endregion