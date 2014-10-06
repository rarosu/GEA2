#pragma once

#include <map>
#include <iostream>
#include <mutex>

template <typename> class Resource;

#ifdef ALL_PUBLIC
	#define PUBLIC public
	#define PRIVATE public
	#define PROTECTED public
#else
	#define PUBLIC public
	#define PRIVATE private
	#define PROTECTED protected
#endif

template <typename T>
struct InternalResource
{
	typedef typename std::map<size_t, InternalResource<T>*>::iterator Iterator;

	T* resource;
	size_t hash;
	int refCount;
};

template <typename T>
class ResourceContainer
{
	template <typename> friend class Resource;
PUBLIC:
	~ResourceContainer();
	InternalResource<T>* AddResource(size_t hash, T* resource);
	InternalResource<T>* GetResource(size_t hash);
	void RemoveResource(size_t hash);

PRIVATE:
	std::map<size_t, InternalResource<T>*> resources;
	std::mutex mutex;
};

template <typename T>
ResourceContainer<T>::~ResourceContainer()
{
	for (auto resource : resources)
	{
		delete resource.second;
	}
}

template <typename T>
InternalResource<T>* ResourceContainer<T>::AddResource(size_t hash, T* resource)
{
	InternalResource<T>* internal = new InternalResource<T>;
	internal->resource = resource;
	internal->hash = hash;
	internal->refCount = 0;

	std::pair<InternalResource<T>::Iterator, bool> result;

	{
		std::lock_guard<std::mutex> lock(mutex);
		result = resources.insert(std::pair<size_t, InternalResource<T>*>(hash, internal));
	}
	

#ifdef _DEBUG
	if (!result.second)
	{
		std::cerr << "Overwriting hash " << std::hex << hash << " containing pointer " << result.first->second->resource << " with " << resource << std::endl;
		delete internal;
		return nullptr;
	}
#endif

	return internal;
}

template <typename T>
InternalResource<T>* ResourceContainer<T>::GetResource(size_t hash)
{
	InternalResource<T>::Iterator itr;

	{
		std::lock_guard<std::mutex> lock(mutex);
		itr = resources.find(hash);
	}
	
	if (itr == resources.end())
	{
		return nullptr;
	}

	return itr->second;
}

template <typename T>
void ResourceContainer<T>::RemoveResource(size_t hash)
{
	InternalResource<T>* internal;
	{
		std::lock_guard<std::mutex> lock(mutex);

		InternalResource<T>::Iterator itr = resources.find(hash);

		internal = itr->second;

		resources.erase(itr);
	}

	delete internal;
}