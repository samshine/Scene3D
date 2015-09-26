
#pragma once

#include "resource.h"

/// \brief Resource proxy of any type
class ResourceObject
{
public:
	ResourceObject()
	{
	}

	template<typename Type>
	ResourceObject(const Resource<Type> &resource)
		: object(resource.handle())
	{
	}

	bool is_null() const { return !object; }

	template<typename Type>
	Resource<Type> cast()
	{
		if (object)
		{
			std::shared_ptr<Resource_Impl<Type> > resource = std::dynamic_pointer_cast<Resource_Impl<Type>>(object);
			if (!resource)
				throw Exception("ResourceObject type mismatch");
			return Resource<Type>(resource);
		}
		else
		{
			throw Exception("ResourceObject is null");
		}
	}

	bool operator<(const ResourceObject &other) const { return object < other.object; }
	bool operator<=(const ResourceObject &other) const { return object <= other.object; }
	bool operator>(const ResourceObject &other) const { return object > other.object; }
	bool operator>=(const ResourceObject &other) const { return object >= other.object; }
	bool operator==(const ResourceObject &other) const { return object == other.object; }
	bool operator!=(const ResourceObject &other) const { return object != other.object; }

private:
	std::shared_ptr<Resource_BaseImpl> object;
};
