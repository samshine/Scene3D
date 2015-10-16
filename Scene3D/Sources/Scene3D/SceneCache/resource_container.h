
#pragma once

#include "resource_object.h"
#include <memory>
#include <map>

class ResourceContainerImpl
{
public:
	std::map<std::string, ResourceObject> resources;
};

class ResourceContainer
{
public:
	ResourceContainer() : impl(std::make_shared<ResourceContainerImpl>())
	{
	}

	template<typename Type>
	Resource<Type> get(const std::string &name)
	{
		auto it = impl->resources.find(name);
		if (it != impl->resources.end())
			return it->second.cast<Type>();

		Resource<Type> resource;
		impl->resources[name] = resource;
		return resource;
	}

private:
	std::shared_ptr<ResourceContainerImpl> impl;
};
