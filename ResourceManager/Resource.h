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
	typedef std::function<void(typename const InternalResource<T>::Iterator&)> DestructorFunction;

	Resource(T* resource, typename const InternalResource<T>::Iterator& internal, ResourceContainer<T>* container, const DestructorFunction& destructor);
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
	typename InternalResource<T>::Iterator internal;
	ResourceContainer<T>* container;
	DestructorFunction destructor;
};




template <typename T>
Resource<T>::Resource(T* resource, typename const InternalResource<T>::Iterator& internal, ResourceContainer<T>* container, const DestructorFunction& destructor)
{
	this->resource = resource;
	this->internal = internal;
	this->container = container;
	this->destructor = destructor;
	
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
	{
		this->internal->second.refCount--;
		if (this->internal->second.refCount == 0)
		{
			destructor(this->internal);
		}
	}
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


template <typename T>
Resource<T>& Resource<T>::operator=(const Resource<T>& resource)
{
	if (this->resource == resource.resource)
		return (*this);

	if (this->internal != container->resources.end())
	{
		this->internal->second.refCount--;
		if (this->internal->second.refCount == 0)
		{
			destructor(this->internal);
		}
	}

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