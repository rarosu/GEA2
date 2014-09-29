#pragma once

#include "ResourceContainer.h"
#include <functional>

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
class Resource
{
PUBLIC:
	//Resource();
	Resource(T* resource, typename const std::map<size_t, InternalResource<T>>::iterator& internal, ResourceContainer<T>* container, const std::function<void(typename const std::map<size_t, InternalResource<T>>::iterator&)>& destructor);
	Resource(const Resource<T>& resource);
	~Resource();

	T* operator->();
	T& operator*();
	Resource<T>& operator=(const Resource<T>& resource);

	bool operator==(const T* resource) const;
	bool operator==(const Resource<T>& resource) const;
	bool operator!=(const T* resource) const;
	bool operator!=(const Resource<T>& resource) const;
PRIVATE:
	T* resource;
	typename std::map<size_t, InternalResource<T>>::iterator internal;
	ResourceContainer<T>* container;
};




/*
template <typename T>
Resource<T>::Resource()
{
	resource = nullptr;
	container = nullptr;
}
*/

template <typename T>
Resource<T>::Resource(T* resource, typename const std::map<size_t, InternalResource<T>>::iterator& internal, ResourceContainer<T>* container, const std::function<void(typename const std::map<size_t, InternalResource<T>>::iterator&)>& destructor)
{
	this->resource = resource;
	this->internal = internal;
	this->container = container;
	
	this->internal->second.refCount++;
}

template <typename T>
Resource<T>::Resource(const Resource<T>& resource)
{
	this->resource = resource.resource;
	this->internal = resource.internal;
	this->container = resource.container;
	
	this->internal->second.refCount++;
}

template <typename T>
Resource<T>::~Resource()
{
	if (this->internal != container->resources.end())
		this->internal->second.refCount--;
}

template <typename T>
T* Resource<T>::operator->()
{
	return this->resource;
}

template <typename T>
T& Resource<T>::operator*()
{
	return *this->resource;
}

/*
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
*/

template <typename T>
Resource<T>& Resource<T>::operator=(const Resource<T>& resource)
{
	if (this->resource == resource.resource)
		return (*this);

	if (this->internal != container->resources.end())
		this->internal->second.refCount--;

	this->resource = resource.resource;
	this->internal = resource.internal;
	this->container = resource.container;
	this->internal->second.refCount++;

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