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
	typedef std::function<void(InternalResource<T>* internal)> DestructorFunction;

	Resource();
	Resource(InternalResource<T>* internal, const DestructorFunction& destructor);
	Resource(const Resource<T>& resource);
	~Resource();

	T* operator->();
	const T* operator->() const;
	T& operator*();
	const T& operator*() const;
	Resource<T>& operator=(const Resource<T>& resource);

	T* Get();
	const T* Get() const;

	bool operator==(const T* resource) const;
	bool operator==(const Resource<T>& resource) const;
	bool operator!=(const T* resource) const;
	bool operator!=(const Resource<T>& resource) const;
PRIVATE:
	T* resource;
	InternalResource<T>* internal;
	DestructorFunction destructor;
};

template <typename T>
Resource<T>::Resource()
{
	this->resource = nullptr;
	this->internal = nullptr;
	this->destructor = nullptr;
}

template <typename T>
Resource<T>::Resource(InternalResource<T>* internal, const DestructorFunction& destructor)
{
	this->resource = internal->resource;
	this->internal = internal;
	this->destructor = destructor;
	this->internal->refCount++;
}

template <typename T>
Resource<T>::Resource(const Resource<T>& resource)
{
	this->resource = resource.resource;
	this->internal = resource.internal;
	this->destructor = resource.destructor;	
	this->internal->refCount++;
}

template <typename T>
Resource<T>::~Resource()
{
	if (this->internal != nullptr)
	{
		this->internal->refCount--;
		if (this->internal->refCount == 0)
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
const T* Resource<T>::operator->() const
{
	return this->resource;
}

template <typename T>
T& Resource<T>::operator*()
{
	return *this->resource;
}

template <typename T>
const T& Resource<T>::operator*() const
{
	return *this->resource;
}

template <typename T>
T* Resource<T>::Get()
{
	return resource;
}

template <typename T>
const T* Resource<T>::Get() const
{
	return resource;
}


template <typename T>
Resource<T>& Resource<T>::operator=(const Resource<T>& resource)
{
	if (this->resource == resource.resource)
		return (*this);

	if (this->internal != nullptr)
	{
		this->internal->refCount--;
		if (this->internal->refCount == 0)
		{
			destructor(this->internal);
		}
	}

	this->resource = resource.resource;
	this->internal = resource.internal;
	this->destructor = resource.destructor;
	this->internal->refCount++;

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