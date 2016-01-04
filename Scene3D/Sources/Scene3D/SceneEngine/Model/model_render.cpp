
#include "precomp.h"
#include "model_render.h"
#include "model_instances_buffer.h"
#include "Scene3D/Scene/scene_object_impl.h"
#include "Scene3D/Scene/scene_light_probe_impl.h"
#include "Scene3D/Scene/scene_impl.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/SceneEngine/Model/model.h"
#include "Scene3D/SceneEngine/Model/model_mesh.h"

using namespace uicore;

void ModelRender::clear(SceneImpl *cur_scene)
{
	scene = cur_scene;
	scene->engine()->render.scene_visits++;

	model_meshes.clear();
	frame++;
}

void ModelRender::add_instance(SceneObjectImpl *object)
{
	object->instance.get_renderer()->create_mesh(scene->engine()->render.gc);

	Vec3f light_probe_color;
	if (object->light_probe_receiver())
	{
		SceneLightProbeImpl *probe = find_nearest_probe(scene, object->position());
		if (probe)
			light_probe_color = probe->color();
	}

	scene->engine()->render.instances_drawn++;

	bool first_instance = object->instance.get_renderer()->mesh->add_instance(frame, object->instance, object->get_object_to_world(), light_probe_color);
	if (first_instance)
	{
		model_meshes.push_back(object->instance.get_renderer()->mesh.get());
		scene->engine()->render.models_drawn++;
	}
}

SceneLightProbeImpl *ModelRender::find_nearest_probe(SceneImpl *scene, const Vec3f &position)
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

void ModelRender::upload(const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection)
{
	ScopeTimeFunction();

	instances_buffer.clear();
	for (auto mesh : model_meshes)
		instances_buffer.add(mesh->get_instance_vectors_count());

	instances_buffer.lock(scene->engine()->render.gc);
	{
		ScopeTimer scope_timer_upload("ModelRender::upload(copy)");
		for (auto mesh : model_meshes)
			mesh->upload(instances_buffer, world_to_eye, eye_to_projection);
	}
	instances_buffer.unlock(scene->engine()->render.gc);
}

void ModelRender::render_gbuffer()
{
	ScopeTimeFunction();

	auto &gc = scene->engine()->render.gc;

	gc->set_texture(0, instances_buffer.get_indexes());
	gc->set_texture(1, instances_buffer.get_vectors());

	for (auto mesh : model_meshes)
		mesh->gbuffer_commands.execute(scene, scene->engine()->render.gc, mesh->instances.size());

	gc->set_texture(0, nullptr);
	gc->set_texture(1, nullptr);
}

void ModelRender::render_transparency()
{
	ScopeTimeFunction();

	auto &gc = scene->engine()->render.gc;

	gc->set_texture(0, instances_buffer.get_indexes());
	gc->set_texture(1, instances_buffer.get_vectors());

	for (auto mesh : model_meshes)
		mesh->transparency_commands.execute(scene, scene->engine()->render.gc, mesh->instances.size());

	gc->set_texture(0, nullptr);
	gc->set_texture(1, nullptr);
}

void ModelRender::render_shadow()
{
	ScopeTimeFunction();

	auto &gc = scene->engine()->render.gc;

	gc->set_texture(0, instances_buffer.get_indexes());
	gc->set_texture(1, instances_buffer.get_vectors());

	for (auto mesh : model_meshes)
		mesh->shadow_commands.execute(scene, scene->engine()->render.gc, mesh->instances.size());

	gc->set_texture(0, nullptr);
	gc->set_texture(1, nullptr);
}

void ModelRender::render_early_z()
{
	ScopeTimeFunction();

	auto &gc = scene->engine()->render.gc;

	gc->set_texture(0, instances_buffer.get_indexes());
	gc->set_texture(1, instances_buffer.get_vectors());

	for (auto mesh : model_meshes)
		mesh->early_z_commands.execute(scene, scene->engine()->render.gc, mesh->instances.size());

	gc->set_texture(0, nullptr);
	gc->set_texture(1, nullptr);
}

int ModelRender::new_model_index()
{
	return instances_buffer.new_offset_index();
}
