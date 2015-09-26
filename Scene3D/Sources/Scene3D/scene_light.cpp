
#include "precomp.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/scene.h"
#include "Passes/VSMShadowMap/vsm_shadow_map_pass.h"
#include "Scene3D/scene_light_impl.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

SceneLight::SceneLight()
{
}

SceneLight::SceneLight(Scene &scene)
	: impl(std::make_shared<SceneLight_Impl>(scene.impl.get()))
{
	impl->cull_proxy = impl->scene->cull_provider->create_proxy(impl.get(), impl->get_aabb());
}

SceneLight::Type SceneLight::get_type() const
{
	return impl->type;
}

uicore::Vec3f SceneLight::get_position() const
{
	return impl->position;
}

uicore::Quaternionf SceneLight::get_orientation() const
{
	return impl->orientation;
}

uicore::Vec3f SceneLight::get_color() const
{
	return impl->color;
}

float SceneLight::get_falloff() const
{
	return impl->falloff;
}

float SceneLight::get_hotspot() const
{
	return impl->hotspot;
}

float SceneLight::get_ambient_illumination() const
{
	return impl->ambient_illumination;
}

float SceneLight::get_attenuation_start() const
{
	return impl->attenuation_start;
}

float SceneLight::get_attenuation_end() const
{
	return impl->attenuation_end;
}

bool SceneLight::is_rectangle_shape() const
{
	return impl->rectangle_shape;
}

float SceneLight::get_aspect_ratio() const
{
	return impl->aspect_ratio;
}

bool SceneLight::is_shadow_caster() const
{
	return impl->shadow_caster;
}

bool SceneLight::is_light_caster() const
{
	return impl->light_caster;
}

SceneLight SceneLight::get_shadow_source() const
{
	return impl->shadow_source;
}

void SceneLight::set_type(Type type)
{
	impl->type = type;
}

void SceneLight::set_position(const uicore::Vec3f &position)
{
	if (impl->position != position)
	{
		impl->position = position;
		if (impl->cull_proxy)
			impl->scene->cull_provider->set_aabb(impl->cull_proxy, impl->get_aabb());
	}
}

void SceneLight::set_orientation(const uicore::Quaternionf &orientation)
{
	impl->orientation = orientation;
}

void SceneLight::set_color(uicore::Vec3f color)
{
	impl->color = color;
}

void SceneLight::set_falloff(float falloff)
{
	impl->falloff = falloff;
}

void SceneLight::set_hotspot(float hotspot)
{
	impl->hotspot = hotspot;
}

void SceneLight::set_ambient_illumination(float factor)
{
	impl->ambient_illumination = factor;
}

void SceneLight::set_attenuation_start(float attenuation_start)
{
	if (impl->attenuation_start != attenuation_start)
	{
		impl->attenuation_start = attenuation_start;
		if (impl->cull_proxy)
			impl->scene->cull_provider->set_aabb(impl->cull_proxy, impl->get_aabb());
	}
}

void SceneLight::set_attenuation_end(float attenuation_end)
{
	if (impl->attenuation_end != attenuation_end)
	{
		impl->attenuation_end = attenuation_end;
		if (impl->cull_proxy)
			impl->scene->cull_provider->set_aabb(impl->cull_proxy, impl->get_aabb());
	}
}

void SceneLight::set_rectangle_shape(bool rectangle)
{
	impl->rectangle_shape = rectangle;
}

void SceneLight::set_aspect_ratio(float aspect)
{
	impl->aspect_ratio = aspect;
}

void SceneLight::set_shadow_caster(bool casts_shadows)
{
	impl->shadow_caster = casts_shadows;
}

void SceneLight::set_light_caster(bool casts_light)
{
	impl->light_caster = casts_light;
}

void SceneLight::set_shadow_source(SceneLight light)
{
	impl->shadow_source = light;
}

/////////////////////////////////////////////////////////////////////////////

SceneLight_Impl::SceneLight_Impl(Scene_Impl *scene)
: scene(scene), cull_proxy(0), type(SceneLight::type_omni), color(Colorf::white), falloff(90.0f),
  hotspot(45.0f), ambient_illumination(0.0f), attenuation_start(1.0f), attenuation_end(100.0f), rectangle_shape(false), aspect_ratio(1.0f), shadow_caster(false), light_caster(true)
{
	it = scene->lights.insert(scene->lights.end(), this);
}

SceneLight_Impl::~SceneLight_Impl()
{
	if (cull_proxy)
		scene->cull_provider->delete_proxy(cull_proxy);
	scene->lights.erase(it);
}

uicore::AxisAlignedBoundingBox SceneLight_Impl::get_aabb()
{
	uicore::AxisAlignedBoundingBox aabb;
	aabb.aabb_min = position - attenuation_end * 1.73205081f;
	aabb.aabb_max = position + attenuation_end * 1.73205081f;
	return aabb;
}
