
#include "precomp.h"
#include "instances_buffer.h"
#include "Scene3D/scene_object_impl.h"
#include "Scene3D/scene_light_probe_impl.h"
#include "Scene3D/scene_impl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Model/model.h"

using namespace uicore;

InstancesBuffer::InstancesBuffer()
	: max_offset_indexes(0), next_offset_index(0), max_vectors(0), num_vectors(0), current_buffer(0)
{
}

int InstancesBuffer::new_offset_index()
{
	return next_offset_index++;
}

void InstancesBuffer::render_pass(const GraphicContextPtr &gc, SceneImpl *scene, const Mat4f &world_to_eye, const Mat4f &eye_to_projection, FrustumPlanes frustum, const std::function<void(ModelLOD*, int)> &pass_callback)
{
	ScopeTimeFunction();
	scene->engine()->render.scene_visits++;

	std::vector<Model *> models;

	scene->foreach_object(frustum, [&](SceneObjectImpl *object)
	{
		if (object->instance.get_renderer())
		{
			Vec3f light_probe_color;
			if (object->light_probe_receiver())
			{
				SceneLightProbeImpl *probe = find_nearest_probe(scene, object->position());
				if (probe)
					light_probe_color = probe->color();
			}

			scene->engine()->render.instances_drawn++;
			bool first_instance = object->instance.get_renderer()->add_instance(frame, object->instance, object->get_object_to_world(), light_probe_color);
			if (first_instance)
			{
				models.push_back(object->instance.get_renderer().get());
				scene->engine()->render.models_drawn++;
			}
		}
	});

	frame++;

	clear();
	for (size_t i = 0; i < models.size(); i++)
		add(models[i]->get_instance_vectors_count());

	lock(gc);
	for (size_t i = 0; i < models.size(); i++)
		models[i]->upload(*this, world_to_eye, eye_to_projection);
	unlock(gc);

	gc->set_texture(0, get_indexes());
	gc->set_texture(1, get_vectors());

	for (Model *model : models)
	{
		pass_callback(model->levels[0].get(), model->instances.size());
	}

	// We cannot reset those because GBufferPass::run contains a hack relying on them being bound
	//gc->set_texture(0, nullptr);
	//gc->set_texture(1, nullptr);
}

SceneLightProbeImpl *InstancesBuffer::find_nearest_probe(SceneImpl *scene, const Vec3f &position)
{
	SceneLightProbeImpl *probe = 0;
	float sqr_distance = 0.0f;

	scene->foreach_light_probe(position, [&](SceneLightProbeImpl *current_probe)
	{
		Vec3f delta = current_probe->position() - position;
		float current_sqr_distance = Vec3f::dot(delta, delta);
		if (probe == 0 || current_sqr_distance < sqr_distance)
		{
			probe = current_probe;
			sqr_distance = current_sqr_distance;
		}
	});

	return probe;
}

void InstancesBuffer::clear()
{
	num_vectors = 0;
	current_buffer = (current_buffer + 1) % num_buffers;
}

void InstancesBuffer::add(int vectors_count)
{
	num_vectors += vectors_count;
}

void InstancesBuffer::lock(const GraphicContextPtr &gc)
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

MappedBuffer<Vec4f> InstancesBuffer::upload(int offset_index, int vectors)
{
	if (offset_index < 0 || offset_index >= indexes_transfer[current_buffer]->width() * indexes_transfer[current_buffer]->height())
	{
		int *foobar = 0;
		*foobar = 0xdeadbabe;
	}

	if (num_vectors + vectors > vectors_transfer[current_buffer]->width() * vectors_transfer[current_buffer]->height())
	{
		int *foobar = 0;
		*foobar = 0xdeadbeef;
	}

	indexes_transfer[current_buffer]->data<float>()[offset_index] = (float)num_vectors;
	Vec4f *v = vectors_transfer[current_buffer]->data<Vec4f>() + num_vectors;
	num_vectors += vectors;
	return MappedBuffer<Vec4f>(v, vectors);
}

void InstancesBuffer::unlock(const GraphicContextPtr &gc)
{
	if (num_vectors == 0)
		return;

	indexes_transfer[current_buffer]->unlock();
	vectors_transfer[current_buffer]->unlock();

	indexes[current_buffer]->set_image(gc, indexes_transfer[current_buffer]);
	vectors[current_buffer]->set_image(gc, vectors_transfer[current_buffer]);
}
