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
	~ResourceContainer();

	InternalResource<T>* AddResource(size_t hash, T* resource);
PRIVATE:
	//static const size_t HASHMAP_SIZE = 10000;
	//InternalResource<T> resources[HASHMAP_SIZE];
	std::map<size_t, InternalResource<T>*> resources;
};







template <typename T>
ResourceContainer<T>::~ResourceContainer()
{
	for (auto it : resources)
	{
		delete it.second;
	}
}

template <typename T>
InternalResource<T>* ResourceContainer<T>::AddResource(size_t hash, T* resource)
{
	InternalResource<T>* internal = new InternalResource<T>;
	internal->resource = resource;
	internal->refCount = 0;

#ifdef _DEBUG
	auto it = resources.find(hash);
	if (it != resources.end())
	{
		std::cerr << "Overwriting hash " << std::hex << hash << " containing pointer " << it->second->resource << " with " << resource << std::endl;
	}
#endif

	resources[hash] = internal;
	return internal;

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