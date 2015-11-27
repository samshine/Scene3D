
#include "precomp.h"
#include "model_instances_buffer.h"
#include "Scene3D/Scene/scene_object_impl.h"
#include "Scene3D/Scene/scene_light_probe_impl.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneEngine/Model/model.h"
#include "Scene3D/SceneEngine/Model/model_mesh.h"

using namespace uicore;

ModelInstancesBuffer::ModelInstancesBuffer()
	: max_offset_indexes(0), next_offset_index(0), max_vectors(0), num_vectors(0), current_buffer(0)
{
}

int ModelInstancesBuffer::new_offset_index()
{
	return next_offset_index++;
}

void ModelInstancesBuffer::clear()
{
	num_vectors = 0;
	current_buffer = (current_buffer + 1) % num_buffers;
}

void ModelInstancesBuffer::add(int vectors_count)
{
	num_vectors += vectors_count;
}

void ModelInstancesBuffer::lock(const GraphicContextPtr &gc)
{
	if (num_vectors == 0)
		return;

	if (num_vectors > max_vectors)
	{
		max_vectors = num_vectors * 2;
		int pixels_width = min(max_vectors, 8192);
		int pixels_height = (max_vectors + 8191) / 8192;
		for (int i = 0; i < num_buffers; i++)
		{
			vectors[i] = Texture2D::create(gc, pixels_width, pixels_height, tf_rgba32f);
			vectors_transfer[i] = StagingTexture::create(gc, pixels_width, pixels_height, StagingDirection::to_gpu, tf_rgba32f, 0, usage_stream_draw);
		}
	}

	if (next_offset_index > max_offset_indexes)
	{
		max_offset_indexes = next_offset_index * 2;
		int pixels_width = min(max_offset_indexes, 1024);
		int pixels_height = (max_offset_indexes + 1024) / 1024;
		for (int i = 0; i < num_buffers; i++)
		{
			indexes[i] = Texture2D::create(gc, pixels_width, pixels_height, tf_r32f);
			indexes_transfer[i] = StagingTexture::create(gc, pixels_width, pixels_height, StagingDirection::to_gpu, tf_r32f, 0, usage_stream_draw);
		}
	}

	indexes_transfer[current_buffer]->lock(gc, access_write_discard);
	vectors_transfer[current_buffer]->lock(gc, access_write_discard);

	num_vectors = 0;
}

MappedBuffer<Vec4f> ModelInstancesBuffer::upload(int offset_index, int vectors)
{
	if (offset_index < 0 || offset_index >= indexes_transfer[current_buffer]->width() * indexes_transfer[current_buffer]->height())
		CrashReporter::invoke();

	if (num_vectors + vectors > vectors_transfer[current_buffer]->width() * vectors_transfer[current_buffer]->height())
		CrashReporter::invoke();

	indexes_transfer[current_buffer]->data<float>()[offset_index] = (float)num_vectors;
	Vec4f *v = vectors_transfer[current_buffer]->data<Vec4f>() + num_vectors;
	num_vectors += vectors;
	return MappedBuffer<Vec4f>(v, vectors);
}

void ModelInstancesBuffer::unlock(const GraphicContextPtr &gc)
{
	if (num_vectors == 0)
		return;

	indexes_transfer[current_buffer]->unlock();
	vectors_transfer[current_buffer]->unlock();

	indexes[current_buffer]->set_image(gc, indexes_transfer[current_buffer]);
	vectors[current_buffer]->set_image(gc, vectors_transfer[current_buffer]);
}
