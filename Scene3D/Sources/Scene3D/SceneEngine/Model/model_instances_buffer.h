
#pragma once

#include "../mapped_buffer.h"

class ModelInstancesBuffer
{
public:
	ModelInstancesBuffer();

	int new_offset_index();

	void clear();
	void add(int vectors_count);

	void lock(const uicore::GraphicContextPtr &gc);
	MappedBuffer<uicore::Vec4f> upload(int offset_index, int vectors);
	void unlock(const uicore::GraphicContextPtr &gc);

	uicore::Texture2DPtr &get_indexes() { return indexes[current_buffer]; }
	uicore::Texture2DPtr &get_vectors() { return vectors[current_buffer]; }

private:
	enum { num_buffers = 16 };
	uicore::StagingTexturePtr indexes_transfer[num_buffers];
	uicore::StagingTexturePtr vectors_transfer[num_buffers];

	int max_offset_indexes;
	int next_offset_index;

	int max_vectors;
	int num_vectors;

	int current_buffer;
	uicore::Texture2DPtr indexes[num_buffers];
	uicore::Texture2DPtr vectors[num_buffers];
};
