
#pragma once

template<typename Type>
class MappedBuffer
{
public:
	MappedBuffer() : ptr(0), length(0) { }
	MappedBuffer(Type *ptr, size_t length) : ptr(ptr), length(length) { }

	Type &operator[](size_t index)
	{
		return (index < length) ? ptr[index] : *((Type*)0);
	}

	const Type &operator[](size_t index) const
	{
		return (index < length) ? ptr[index] : *((Type*)0);
	}

	MappedBuffer operator+(size_t offset) const
	{
		if (length < offset)
			return MappedBuffer();
		else
			return MappedBuffer(ptr + offset, length - offset);
	}

private:
	Type *ptr;
	size_t length;
};
