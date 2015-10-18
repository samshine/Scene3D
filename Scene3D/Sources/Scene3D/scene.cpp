
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

std::shared_ptr<Scene> Scene::create(const SceneCachePtr &cache)
{
	auto scene = std::make_shared<SceneImpl>(cache);
	scene->set_camera(SceneCamera::create(scene));
	return scene;
}

SceneImpl::SceneImpl(const SceneCachePtr &cache) : cache(std::dynamic_pointer_cast<SceneCacheImpl>(cache))
{
	cull_provider = std::unique_ptr<SceneCullProvider>(new OctTree());

	auto shader_path = get_cache()->get_shader_path();
	auto gc = get_cache()->get_gc();

	material_cache = std::unique_ptr<MaterialCache>(new MaterialCache(this));
	model_shader_cache = std::unique_ptr<ModelShaderCache>(new ModelShaderCache(shader_path));
	model_cache = std::unique_ptr<ModelCache>(new ModelCache(this, *material_cache, *model_shader_cache, instances_buffer));

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
	passes.push_back(std::make_shared<LensFlarePass>(shader_path, inout_data));
	passes.push_back(std::make_shared<BloomPass>(gc, shader_path, inout_data));
	//passes.push_back(std::make_shared<SSAOPass>(gc, shader_path, inout_data));
	passes.push_back(std::make_shared<FinalPass>(gc, shader_path, inout_data));
}

Mat4f SceneImpl::world_to_eye() const
{
	Quaternionf inv_orientation = Quaternionf::inverse(_camera->orientation());
	return inv_orientation.to_matrix() * Mat4f::translate(_camera->position());
}

Mat4f SceneImpl::eye_to_projection() const
{
	Size viewport_size = inout_data.viewport.size();
	return Mat4f::perspective(inout_data.field_of_view, viewport_size.width/(float)viewport_size.height, 0.1f, 1.e10f, handed_left, clip_negative_positive_w);
}

Mat4f SceneImpl::world_to_projection() const
{
	return eye_to_projection() * world_to_eye();
}

void SceneImpl::unproject(const Vec2i &screen_pos, Vec3f &out_ray_start, Vec3f &out_ray_direction)
{
	Size viewport_size = inout_data.viewport.size();

	float aspect = inout_data.viewport.width() / (float)inout_data.viewport.height();
	float field_of_view_y_degrees = inout_data.field_of_view;
	float field_of_view_y_rad = (float)(field_of_view_y_degrees * PI / 180.0);
	float f = 1.0f / tan(field_of_view_y_rad * 0.5f);
	float rcp_f = 1.0f / f;
	float rcp_f_div_aspect = 1.0f / (f / aspect);

	Vec2f pos((float)(screen_pos.x - inout_data.viewport.left), (float)(inout_data.viewport.bottom - screen_pos.y));

	Vec2f normalized(pos.x * 2.0f / inout_data.viewport.width(), pos.y * 2.0f / inout_data.viewport.height());
	normalized -= 1.0f;

	Vec3f ray_direction(normalized.x * rcp_f_div_aspect, normalized.y * rcp_f, 1.0f);

	out_ray_start = _camera->position();
	out_ray_direction = _camera->orientation().rotate_vector(ray_direction);
}

void SceneImpl::set_cull_oct_tree(const AxisAlignedBoundingBox &aabb)
{
	cull_provider = std::unique_ptr<SceneCullProvider>(new OctTree(aabb));
}

void SceneImpl::set_cull_oct_tree(const Vec3f &aabb_min, const Vec3f &aabb_max)
{
	if (!objects.empty() || !lights.empty() || !emitters.empty() || !light_probes.empty())
		throw Exception("Cannot change scene culling strategy after objects have been added");

	set_cull_oct_tree(AxisAlignedBoundingBox(aabb_min, aabb_max));
}

void SceneImpl::set_cull_oct_tree(float max_size)
{
	set_cull_oct_tree(AxisAlignedBoundingBox(Vec3f(-max_size), Vec3f(max_size)));
}

void SceneImpl::show_skybox_stars(bool enable)
{
	inout_data.show_skybox_stars = enable;
}

void SceneImpl::set_skybox_gradient(const GraphicContextPtr &gc, std::vector<Colorf> &colors)
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

	inout_data.skybox_texture = texture;
}

void SceneImpl::set_viewport(const Rect &box, const FrameBufferPtr &fb)
{
	inout_data.viewport = box;
	inout_data.fb_viewport = fb;
}

void SceneImpl::render(const GraphicContextPtr &gc)
{
	ScopeTimeFunction();

	_models_drawn = 0;
	_instances_drawn = 0;
	_draw_calls = 0;
	_triangles_drawn = 0;
	_scene_visits = 0;

	gpu_timer.begin_frame(gc);

	inout_data.setup_pass_buffers(gc);

	if (inout_data.field_of_view != _camera->field_of_view())
		inout_data.field_of_view = _camera->field_of_view();

	Quaternionf inv_orientation = Quaternionf::inverse(_camera->orientation());
	inout_data.world_to_eye = inv_orientation.to_matrix() * Mat4f::translate(-_camera->position());

	for (const auto &pass : passes)
	{
		gpu_timer.begin_time(gc, pass->name());
		pass->run(gc, this);
		gpu_timer.end_time(gc);
	}

	gpu_timer.end_frame(gc);

	_gpu_results = gpu_timer.get_results(gc);

	if (gc->shader_language() == shader_glsl)
		OpenGL::check_error();
}

void SceneImpl::update(const GraphicContextPtr &gc, float time_elapsed)
{
	get_cache()->process_work_completed();
	material_cache->update(gc, time_elapsed);
	for (const auto &pass : passes)
		pass->update(gc, time_elapsed);
	// To do: update scene object animations here too
}

void SceneImpl::foreach(const FrustumPlanes &frustum, const std::function<void(SceneItem *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
		callback(item);
}

void SceneImpl::foreach(const Vec3f &position, const std::function<void(SceneItem *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
		callback(item);
}

void SceneImpl::foreach_object(const FrustumPlanes &frustum, const std::function<void(SceneObjectImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
	{
		SceneObjectImpl *v = dynamic_cast<SceneObjectImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_object(const Vec3f &position, const std::function<void(SceneObjectImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
	{
		SceneObjectImpl *v = dynamic_cast<SceneObjectImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_light(const FrustumPlanes &frustum, const std::function<void(SceneLightImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
	{
		SceneLightImpl *v = dynamic_cast<SceneLightImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_light(const Vec3f &position, const std::function<void(SceneLightImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
	{
		SceneLightImpl *v = dynamic_cast<SceneLightImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_light_probe(const FrustumPlanes &frustum, const std::function<void(SceneLightProbeImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
	{
		SceneLightProbeImpl *v = dynamic_cast<SceneLightProbeImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_light_probe(const Vec3f &position, const std::function<void(SceneLightProbeImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
	{
		SceneLightProbeImpl *v = dynamic_cast<SceneLightProbeImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_emitter(const FrustumPlanes &frustum, const std::function<void(SceneParticleEmitterImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(frustum))
	{
		SceneParticleEmitterImpl *v = dynamic_cast<SceneParticleEmitterImpl*>(item);
		if (v)
			callback(v);
	}
}

void SceneImpl::foreach_emitter(const Vec3f &position, const std::function<void(SceneParticleEmitterImpl *)> &callback)
{
	ScopeTimeFunction();
	for (SceneItem *item : cull_provider->cull(position))
	{
		SceneParticleEmitterImpl *v = dynamic_cast<SceneParticleEmitterImpl*>(item);
		if (v)
			callback(v);
	}
}
