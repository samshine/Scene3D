
#pragma once

#include <memory>
#include <map>

class Resource_BaseImpl
{
public:
	virtual ~Resource_BaseImpl() { }
};

template<typename Type>
class Resource_Impl : public Resource_BaseImpl
{
public:
	Resource_Impl() : value(), generation(0) { }
	Resource_Impl(const Type &initial_value) : value(initial_value), generation(0) { }
	Type value;
	int generation;
};

/// \brief Resource proxy of a specific type
template<typename Type>
class Resource
{
public:
	Resource()
		: object(new Resource_Impl<Type>()), generation(-1)
	{
	}

	Resource(std::shared_ptr<Resource_Impl<Type> > object)
		: object(object), generation(-1)
	{
	}

	Resource(const Type &initial_value)
		: object(new Resource_Impl<Type>(initial_value)), generation(-1)
	{
	}

	Type *operator->()
	{
		return &object->value;
	}

	const Type *operator->() const
	{
		return &object->value;
	}

	bool updated()
	{
		bool updated = (generation != object->generation);
		generation = object->generation;
		return updated;
	}

	void set(const Type &value)
	{
		object->value = value;
		generation = ++object->generation;
	}

	Type &get()
	{
		return object->value;
	}

	const Type &get() const
	{
		return object->value;
	}

	operator Type&() { return object->value; }
	operator const Type&() const { return object->value; }

	const std::shared_ptr<Resource_Impl<Type> > &handle() const { return object; }

	bool operator<(const Resource &other) const { return object < other.object; }
	bool operator<=(const Resource &other) const { return object <= other.object; }
	bool operator>(const Resource &other) const { return object > other.object; }
	bool operator>=(const Resource &other) const { return object >= other.object; }
	bool operator==(const Resource &other) const { return object == other.object; }
	bool operator!=(const Resource &other) const { return object != other.object; }

private:
	std::shared_ptr<Resource_Impl<Type> > object;
	int generation;
};
