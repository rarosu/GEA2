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
	T* resource;
	int refCount;
};

template <typename T>
class ResourceContainer
{
PUBLIC:
	typename std::map<size_t, InternalResource<T>>::iterator AddResource(size_t hash, T* resource);
PRIVATE:
	//static const size_t HASHMAP_SIZE = 10000;
	//InternalResource<T> resources[HASHMAP_SIZE];
	//std::map<size_t, InternalResource<T>*> resources;
	std::map<size_t, InternalResource<T>> resources;
};







template <typename T>
typename std::map<size_t, InternalResource<T>>::iterator ResourceContainer<T>::AddResource(size_t hash, T* resource)
{
	InternalResource<T> internal;
	internal.resource = resource;
	internal.refCount = 0;

	std::pair<std::map<size_t, InternalResource<T>>::iterator, bool> result = resources.insert(std::pair<size_t, InternalResource<T>>(hash, internal));

#ifdef _DEBUG
	if (!result.second)
	{
		std::cerr << "Overwriting hash " << std::hex << hash << " containing pointer " << result.first->second.resource << " with " << resource << std::endl;
		return resources.end();
	}
#endif

	return result.first;

	/*
	// DEPRECATED - Hash map impl. //

	size_t index = hash % HASHMAP_SIZE;
#ifdef _DEBUG
	if (resources[index].resource != nullptr)
		std::cerr << "Overwriting hash " << std::hex << hash << " containing pointer " << resources[index].resource << " with " << resource << std::endl;
#endif

	resources[hash % HASHMAP_SIZE] = internal;
	return &resources[hash];
	*/
}