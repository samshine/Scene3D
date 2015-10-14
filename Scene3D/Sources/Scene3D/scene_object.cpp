
#include "precomp.h"
#include "Scene3D/scene_object.h"
#include "Scene3D/scene_model.h"
#include "Scene3D/scene.h"
#include <algorithm>
#include "Scene3D/scene_object_impl.h"
#include "Scene3D/Model/model.h"
#include "Scene3D/scene_impl.h"
#include "Scene3D/scene_model_impl.h"

using namespace uicore;

std::shared_ptr<SceneObject> SceneObject::create(Scene &scene, const SceneModelPtr &model, const Vec3f &position, const Quaternionf &orientation, const Vec3f &scale)
{
	auto impl = std::make_shared<SceneObject_Impl>(scene.impl.get());
	impl->_position = position;
	impl->_orientation = orientation;
	impl->_scale = scale;
	impl->instance.set_renderer(static_cast<SceneModel_Impl*>(model.get())->model);
	impl->cull_proxy = impl->scene->cull_provider->create_proxy(impl.get(), impl->get_aabb());

	impl->create_lights(scene);

	return impl;
}

SceneObject_Impl::SceneObject_Impl(Scene_Impl *scene) : scene(scene)
{
	it = scene->objects.insert(scene->objects.end(), this);
}

SceneObject_Impl::~SceneObject_Impl()
{
	if (cull_proxy)
		scene->cull_provider->delete_proxy(cull_proxy);
	scene->objects.erase(it);
}

void SceneObject_Impl::set_position(const Vec3f &position)
{
	if (_position != position)
	{
		_position = position;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
		update_lights();
	}
}

void SceneObject_Impl::set_orientation(const Quaternionf &orientation)
{
	_orientation = orientation;
	update_lights();
}

void SceneObject_Impl::set_scale(const Vec3f &scale)
{
	if (_scale != scale)
	{
		_scale = scale;
		if (cull_proxy)
			scene->cull_provider->set_aabb(cull_proxy, get_aabb());
		update_lights();
	}
}

void SceneObject_Impl::set_light_probe_receiver(bool enable)
{
	_light_probe_receiver = enable;
}

std::string SceneObject_Impl::animation() const
{
	return instance.get_animation();
}

void SceneObject_Impl::play_animation(const std::string &name, bool instant)
{
	instance.play_animation(name, instant);
	update_lights();
}

void SceneObject_Impl::play_transition(const std::string &anim1, const std::string &anim2, bool instant)
{
	instance.play_transition(anim1, anim2, instant);
	update_lights();
}

void SceneObject_Impl::update(float time_elapsed)
{
	instance.update(time_elapsed);
	update_lights();
}

void SceneObject_Impl::moved(float units_moved)
{
	instance.moved(units_moved / _scale.y);
	update_lights();
}

void SceneObject_Impl::move(Vec3f offset)
{
	set_position(position() + orientation().rotate_vector(offset));
}

void SceneObject_Impl::rotate(float dir, float up, float tilt)
{
	set_orientation(orientation() * Quaternionf(up, dir, tilt, angle_degrees, order_YXZ));
}

void SceneObject_Impl::attachment_location(const std::string &name, Vec3f &attach_position, Quaternionf &attach_orientation, Vec3f &attach_scale) const
{
	Vec3f local_position;
	Quaternionf local_orientation;
	instance.get_attachment_location(name, local_position, local_orientation);
	attach_position = position() + orientation().rotate_vector(local_position) * scale();
	attach_orientation = orientation() * local_orientation;
	attach_scale = scale();
}

void SceneObject_Impl::create_lights(Scene &scene_base)
{
	std::vector<ModelDataLight> &model_lights = instance.get_renderer()->get_model_data()->lights;
	for (size_t i = 0; i < model_lights.size(); i++)
		lights.push_back(SceneLight::create(scene_base));
	update_lights();
}

void SceneObject_Impl::update_lights()
{
	Mat4f object_to_world = get_object_to_world();

	int animation_index = instance.get_animation_index();
	float animation_time = instance.get_animation_time();
	if (animation_index != -1)
	{
		std::vector<ModelDataLight> &model_lights = instance.get_renderer()->get_model_data()->lights;
		for (size_t i = 0; i < model_lights.size(); i++)
		{
			// To do: apply bone_selector
			lights[i]->set_type(model_lights[i].falloff.get_value(animation_index, animation_time) == 0.0f ? SceneLight::type_omni : SceneLight::type_spot);
			lights[i]->set_position(Vec3f(object_to_world * Vec4f(model_lights[i].position.get_value(animation_index, animation_time), 1.0f)));
			lights[i]->set_orientation(orientation() * model_lights[i].orientation.get_value(animation_index, animation_time));
			lights[i]->set_attenuation_start(model_lights[i].attenuation_start.get_value(animation_index, animation_time) * _scale.y);
			lights[i]->set_attenuation_end(model_lights[i].attenuation_end.get_value(animation_index, animation_time) * _scale.y);
			lights[i]->set_color(model_lights[i].color.get_value(animation_index, animation_time));
			lights[i]->set_aspect_ratio(model_lights[i].aspect.get_value(animation_index, animation_time));
			lights[i]->set_falloff(model_lights[i].falloff.get_value(animation_index, animation_time));
			lights[i]->set_hotspot(model_lights[i].hotspot.get_value(animation_index, animation_time));
			lights[i]->set_ambient_illumination(model_lights[i].ambient_illumination.get_value(animation_index, animation_time));
			lights[i]->set_shadow_caster(model_lights[i].casts_shadows);
			lights[i]->set_light_caster(true);
			lights[i]->set_rectangle_shape(model_lights[i].rectangle);
		}
	}
}

AxisAlignedBoundingBox SceneObject_Impl::get_aabb() const
{
	if (instance.get_renderer())
		return AxisAlignedBoundingBox(instance.get_renderer()->get_model_data()->aabb_min * _scale + _position, instance.get_renderer()->get_model_data()->aabb_max * _scale + _position);
	else
		return AxisAlignedBoundingBox();
}
