#pragma once

#include <map>
#include <string>
#include <functional>



class ResourceBase
{
public:

private:
	int refCount;
};



template <typename T>
class Resource
{
public:
	Resource();
	virtual ~Resource();

	T* operator->();
	T& operator*();
private:
	ResourceBase* resource;
};



class ResourceManager
{
public:
	template <typename T>
	Resource<T> Load(const std::string& path, std::function<void()>& processor);

	template <typename T>
	Resource<T> Create(const std::string& name);
private:
	// Map a hash to a resource.
	std::map<size_t, ResourceBase> resources;
};




#pragma region Resource

template <typename T>
Resource<T>::Resource()
{

}

template <typename T>
Resource<T>::~Resource()
{

}

template <typename T>
T* Resource<T>::operator->()
{
	return static_cast<T*>(resource);
}

template <typename T>
T& Resource<T>::operator*()
{
	return *static_cast<T*>(resource);
}

#pragma endregion


#pragma region ResourceManager

template <typename T>
Resource<T> ResourceManager::Load(const std::string& path, std::function<void()>& processor)
{
	return Resource<T>();
}

template <typename T>
Resource<T> ResourceManager::Create(const std::string& name)
{
	return Resource<T>();
}

#pragma endregion