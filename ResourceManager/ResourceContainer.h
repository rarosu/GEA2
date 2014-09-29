#pragma once

#include <map>
#include <iostream>

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
	typedef typename std::map<size_t, InternalResource<T>>::iterator Iterator;

	T* resource;
	int refCount;
};

template <typename T>
class ResourceContainer
{
PUBLIC:
	typename InternalResource<T>::Iterator AddResource(size_t hash, T* resource);
	void RemoveResource(const typename InternalResource<T>::Iterator& iterator);
PRIVATE:
	std::map<size_t, InternalResource<T>> resources;
};





template <typename T>
typename InternalResource<T>::Iterator ResourceContainer<T>::AddResource(size_t hash, T* resource)
{
	InternalResource<T> internal;
	internal.resource = resource;
	internal.refCount = 0;

	std::pair<InternalResource<T>::Iterator, bool> result = resources.insert(std::pair<size_t, InternalResource<T>>(hash, internal));

#ifdef _DEBUG
	if (!result.second)
	{
		std::cerr << "Overwriting hash " << std::hex << hash << " containing pointer " << result.first->second.resource << " with " << resource << std::endl;
		return resources.end();
	}
#endif

	return result.first;
}


template <typename T>
void ResourceContainer<T>::RemoveResource(const typename InternalResource<T>::Iterator& iterator)
{
	resources.erase(iterator);
}