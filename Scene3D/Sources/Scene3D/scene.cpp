
#include "precomp.h"
#include "Scene3D/scene.h"
#include "Scene3D/scene_object.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene_particle_emitter.h"
#include "Scene3D/Performance/scope_timer.h"
#include "Scene3D/Culling/OctTree/oct_tree.h"
#include "scene_impl.h"
#include "scene_object_impl.h"
#include "scene_light_probe_impl.h"

using namespace uicore;

Scene::Scene()
{
}

Scene::Scene(const SceneCachePtr &cache)
: impl(std::make_shared<Scene_Impl>(cache))
{
	impl->set_camera(SceneCamera::create(*this));
}

bool Scene::is_null() const
{
	return !impl;
}

const SceneCameraPtr &Scene::get_camera() const
{
	return impl->get_camera();
}

void Scene::set_viewport(const Rect &box, const FrameBufferPtr &fb)
{
	impl->set_viewport(box, fb);
}

void Scene::set_camera(const SceneCameraPtr &camera)
{
	impl->set_camera(camera);
}

void Scene::render(const GraphicContextPtr &gc)
{
	impl->render(gc);
}

void Scene::update(const GraphicContextPtr &gc, float time_elapsed)
{
	impl->update(gc, time_elapsed);
}

Mat4f Scene::world_to_eye() const
{
	Quaternionf inv_orientation = Quaternionf::inverse(impl->camera->orientation());
	return inv_orientation.to_matrix() * Mat4f::translate(-impl->camera->position());
}

Mat4f Scene::eye_to_projection() const
{
	Size viewport_size = impl->viewport->size();
	return Mat4f::perspective(impl->camera_field_of_view.get(), viewport_size.width/(float)viewport_size.height, 0.1f, 1.e10f, handed_left, clip_negative_positive_w);
}

Mat4f Scene::world_to_projection() const
{
	return eye_to_projection() * world_to_eye();
}

void Scene::unproject(const Vec2i &screen_pos, Vec3f &out_ray_start, Vec3f &out_ray_direction)
{
	Size viewport_size = impl->viewport->size();

	float aspect = impl->viewport->width()/(float)impl->viewport->height();
	float field_of_view_y_degrees = impl->camera_field_of_view.get();
	float field_of_view_y_rad = (float)(field_of_view_y_degrees * PI / 180.0);
	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
	float rcp_f = 1.0f / f;
	float rcp_f_div_aspect = 1.0f / (f / aspect);

	Vec2f pos((float)(screen_pos.x - impl->viewport->left), (float)(impl->viewport->bottom - screen_pos.y));

	Vec2f normalized(pos.x * 2.0f / impl->viewport->width(), pos.y * 2.0f / impl->viewport->height());
	normalized -= 1.0f;

	Vec3f ray_direction(normalized.x * rcp_f_div_aspect, normalized.y * rcp_f, 1.0f);

	out_ray_start = impl->camera->position();
	out_ray_direction = impl->camera->orientation().rotate_vector(ray_direction);
}

void Scene::set_cull_oct_tree(const AxisAlignedBoundingBox &aabb)
{
	impl->cull_provider = std::unique_ptr<SceneCullProvider>(new OctTree(aabb));
}

void Scene::set_cull_oct_tree(const Vec3f &aabb_min, const Vec3f &aabb_max)
{
	if (!impl->objects.empty() || !impl->lights.empty() || !impl->emitters.empty() || !impl->light_probes.empty())
		throw Exception("Cannot change scene culling strategy after objects have been added");

	set_cull_oct_tree(AxisAlignedBoundingBox(aabb_min, aabb_max));
}

void Scene::set_cull_oct_tree(float max_size)
{
	set_cull_oct_tree(AxisAlignedBoundingBox(Vec3f(-max_size), Vec3f(max_size)));
}

void Scene::show_skybox_stars(bool enable)
{
	impl->show_skybox_stars.set(enable);
}

void Scene::set_skybox_gradient(const GraphicContextPtr &gc, std::vector<Colorf> &colors)
{
	auto pb = PixelBuffer::create(1, colors.size(), tf_rgba32f);
	Vec4f *pixels = pb->data<Vec4f>();

	for (size_t i = 0; i < colors.size(); i++)
	{
		pixels[i] = Vec4f(colors[i].r, colors[i].g, colors[i].b, colors[i].a);
	}

	auto texture = Texture2D::create(gc, pb->size(), tf_rgba32f);
	texture->set_image(gc, pb);
	texture->set_min_filter(filter_linear);
	texture->set_mag_filter(filter_linear);

	impl->skybox_texture.set(texture);
}

int Scene::models_drawn() const
{
	return impl->models_drawn;
}

int Scene::instances_drawn() const
{
	return impl->instances_drawn;
}

int Scene::draw_calls() const
{
	return impl->draw_calls;
}

int Scene::triangles_drawn() const
{
	return impl->triangles_drawn;
}

int Scene::scene_visits() const
{
	return impl->scene_visits;
}

const std::vector<GPUTimer::Result> &Scene::gpu_results() const
{
	return impl->gpu_results;
}

/////////////////////////////////////////////////////////////////////////////

Scene_Impl::Scene_Impl(const SceneCachePtr &cache) : cache(std::dynamic_pointer_cast<SceneCacheImpl>(cache))
{
	cull_provider = std::unique_ptr<SceneCullProvider>(new OctTree());

	auto shader_path = get_cache()->get_shader_path();
	auto gc = get_cache()->get_gc();

	material_cache = std::unique_ptr<MaterialCache>(new MaterialCache(this));
	model_shader_cache = std::unique_ptr<ModelShaderCache>(new ModelShaderCache(shader_path));
	model_cache = std::unique_ptr<ModelCache>(new ModelCache(this, *material_cache, *model_shader_cache, instances_buffer));

	viewport_fb = inout_data.get<FrameBufferPtr>("ViewportFrameBuffer");
	viewport = inout_data.get<Rect>("Viewport");
	camera_field_of_view = inout_data.get<float>("FieldOfView");
	out_world_to_eye = inout_data.get<Mat4f>("WorldToEye");

	skybox_texture = inout_data.get<Texture2DPtr>("SkyboxTexture");
	show_skybox_stars = inout_data.get<bool>("ShowSkyboxStars");

	viewport.set(Size(640, 480));
	camera_field_of_view.set(60.0f);

	bool use_compute_shader_pass = true;

	if (gc->shader_language() == shader_glsl) // Compute shaders introduced in OpenGL 4.3
	{
		use_compute_shader_pass = gc->has_compute_shader_support();
	}
	else if (gc->shader_language() == shader_hlsl) // We need compute shaders of at least Direct3D feature level 11
	{
		use_compute_shader_pass = gc->major_version() > 10;
	}

	// use_compute_shader_pass = false; // Disable because it crashes with Oculus Rift

	passes.push_back(std::make_shared<GBufferPass>(inout_data));
	passes.push_back(std::make_shared<SkyboxPass>(shader_path, inout_data));
	passes.push_back(std::make_shared<VSMShadowMapPass>(gc, inout_data));

	if (use_compute_shader_pass)
	{
		passes.push_back(std::make_shared<LightsourcePass>(gc, shader_path, inout_data));
	}
	else
	{
		passes.push_back(std::make_shared<LightsourceSimplePass>(gc, shader_path, inout_data));
	}

	passes.push_back(std::make_shared<TransparencyPass>(inout_data));
	passes.push_back(std::make_shared<ParticleEmitterPass>(*material_cache, shader_path, inout_data));
	passes.push_back(std::make_shared<BloomPass>(gc, shader_path, inout_data));
	//passes.push_back(std::make_shared<SSAOPass>(gc, shader_path, inout_data));
	passes.push_back(std::make_shared<FinalPass>(gc, shader_path, inout_data));
}

void Scene_Impl::set_viewport(const Rect &box, const FrameBufferPtr &fb)
{
	viewport.set(box);
	viewport_fb.set(fb);
}

void Scene_Impl::render(const GraphicContextPtr &gc)
{
	ScopeTimeFunction();

	models_drawn = 0;
	instances_drawn = 0;
	draw_calls = 0;
	triangles_drawn = 0;
	scene_visits = 0;

	gpu_timer.begin_frame(gc);

	if (camera_field_of_view.get() != camera->field_of_view())
		camera_field_of_view.set(camera->field_of_view());

	Quaternionf inv_orientation = Quaternionf::inverse(camera->orientation());
	Mat4f world_to_eye = inv_orientation.to_matrix() * Mat4f::translate(-camera->position());

	out_world_to_eye.set(world_to_eye);

	for (const auto &pass : passes)
	{
		gpu_timer.begin_time(gc, pass->name());
		pass->run(gc, this);
		gpu_timer.end_time(gc);
	}

	gpu_timer.end_frame(gc);

	gpu_results = gpu_timer.get_results(gc);

	if (gc->shader_language() == shader_glsl)
		OpenGL::check_error();
}

void Scene_Impl::update(const GraphicContextPtr &gc, float time_elapsed)
{
	get_cache()->process_work_completed();
	material_cache->update(gc, time_elapsed);
	for (const auto &pass : passes)
		pass->update(gc, time_elapsed);
	// To do: update scene object animations here too
}

void Scene_Impl::visit(const GraphicContextPtr &gc, const Mat4f &world_to_eye, const Mat4f &eye_to_projection, FrustumPlanes frustum, ModelMeshVisitor *visitor)
{
	ScopeTimeFunction();
	scene_visits++;

	std::vector<Model *> models;

	std::vector<SceneItem *> visible_objects = cull_provider->cull(frustum);
	for (size_t i = 0; i < visible_objects.size(); i++)
	{
		SceneObject_Impl *object = dynamic_cast<SceneObject_Impl*>(visible_objects[i]);
		if (object)
		{
			if (object->instance.get_renderer())
			{
				Vec3f light_probe_color;
				if (object->light_probe_receiver())
				{
					SceneLightProbe_Impl *probe = find_nearest_probe(object->position());
					if (probe)
						light_probe_color = probe->color();
				}

				instances_drawn++;
				bool first_instance = object->instance.get_renderer()->add_instance(frame, object->instance, object->get_object_to_world(), light_probe_color);
				if (first_instance)
				{
					models.push_back(object->instance.get_renderer().get());
					models_drawn++;
				}
			}
		}
	}

	frame++;

	instances_buffer.clear();
	for (size_t i = 0; i < models.size(); i++)
		instances_buffer.add(models[i]->get_instance_vectors_count());

	instances_buffer.lock(gc);
	for (size_t i = 0; i < models.size(); i++)
		models[i]->upload(instances_buffer, world_to_eye, eye_to_projection);
	instances_buffer.unlock(gc);

	for (size_t i = 0; i < models.size(); i++)
		models[i]->visit(gc, instances_buffer, visitor);
}

void Scene_Impl::visit_lights(const GraphicContextPtr &gc, const Mat4f &world_to_eye, const Mat4f &eye_to_projection, FrustumPlanes frustum, SceneLightVisitor *visitor)
{
	ScopeTimeFunction();

	std::vector<SceneItem *> visible_objects = cull_provider->cull(frustum);
	for (size_t i = 0; i < visible_objects.size(); i++)
	{
		SceneLight_Impl *light = dynamic_cast<SceneLight_Impl*>(visible_objects[i]);
		if (light)
		{
			visitor->light(gc, world_to_eye, eye_to_projection, light);
		}
	}
}

void Scene_Impl::visit_emitters(const GraphicContextPtr &gc, const Mat4f &world_to_eye, const Mat4f &eye_to_projection, FrustumPlanes frustum, SceneParticleEmitterVisitor *visitor)
{
	ScopeTimeFunction();

	std::vector<SceneItem *> visible_objects = cull_provider->cull(frustum);
	for (size_t i = 0; i < visible_objects.size(); i++)
	{
		SceneParticleEmitter_Impl *emitter = dynamic_cast<SceneParticleEmitter_Impl*>(visible_objects[i]);
		if (emitter)
		{
			visitor->emitter(gc, world_to_eye, eye_to_projection, emitter);
		}
	}
}

SceneLightProbe_Impl *Scene_Impl::find_nearest_probe(const Vec3f &position)
{
	SceneLightProbe_Impl *probe = 0;
	float sqr_distance = 0.0f;

	std::vector<SceneItem *> visible_objects = cull_provider->cull(position);
	for (size_t i = 0; i < visible_objects.size(); i++)
	{
		SceneLightProbe_Impl *current_probe = dynamic_cast<SceneLightProbe_Impl*>(visible_objects[i]);
		if (current_probe)
		{
			Vec3f delta = current_probe->position() - position;
			float current_sqr_distance = Vec3f::dot(delta, delta);
			if (probe == 0 || current_sqr_distance < sqr_distance)
			{
				probe = current_probe;
				sqr_distance = current_sqr_distance;
			}
		}
	}

	return probe;
}
