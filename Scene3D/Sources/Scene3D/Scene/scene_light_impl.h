
#pragma once

#include "Scene3D/scene_light.h"
#include "Scene3D/Culling/scene_cull_provider.h"
#include <list>

class SceneImpl;

class SceneLightImpl : public SceneLight, public SceneItem, public std::enable_shared_from_this<SceneLightImpl>
{
public:
	SceneLightImpl(SceneImpl *scene);
	~SceneLightImpl();

	Type type() const override { return _type; }
	uicore::Vec3f position() const override { return _position; }
	uicore::Quaternionf orientation() const override { return _orientation; }

	uicore::Vec3f color() const override { return _color; }
	float falloff() const override { return _falloff; }
	float hotspot() const override { return _hotspot; }
	float ambient_illumination() const override { return _ambient_illumination; }
	float attenuation_start() const override { return _attenuation_start; }
	float attenuation_end() const override { return _attenuation_end; }
	bool rectangle_shape() const override { return _rectangle_shape; }
	float aspect_ratio() const override { return _aspect_ratio; }

	bool shadow_caster() const override { return _shadow_caster; }
	bool light_caster() const override { return _light_caster; }
	std::shared_ptr<SceneLight> shadow_source() const override { return _shadow_source; }

	void set_type(Type type) override { _type = type; }
	void set_position(const uicore::Vec3f &position) override;
	void set_orientation(const uicore::Quaternionf &orientation) override;

	void set_color(uicore::Vec3f color) override { _color = color; }
	void set_falloff(float falloff) override { _falloff = falloff; }
	void set_hotspot(float hotspot) override { _hotspot = hotspot; }
	void set_ambient_illumination(float factor) override { _ambient_illumination = factor; }
	void set_attenuation_start(float attenuation_start) override;
	void set_attenuation_end(float attenuation_end) override;
	void set_rectangle_shape(bool rectangle) override { _rectangle_shape = rectangle; }
	void set_aspect_ratio(float aspect) override { _aspect_ratio = aspect; }

	void set_shadow_caster(bool casts_shadows) override { _shadow_caster = casts_shadows; }
	void set_light_caster(bool casts_light) override { _light_caster = casts_light; }
	void set_shadow_source(const std::shared_ptr<SceneLight> &light) override { _shadow_source = light; }

	uicore::AxisAlignedBoundingBox get_aabb();

	SceneImpl *scene = nullptr;
	SceneCullProxy *cull_proxy = nullptr;
	std::list<SceneLightImpl *>::iterator it;

	SceneLight::Type _type = SceneLight::type_omni;
	uicore::Vec3f _position;
	uicore::Quaternionf _orientation;

	uicore::Vec3f _color = uicore::Vec3f(uicore::Colorf::white);
	float _falloff = 90.0f;
	float _hotspot = 45.0f;
	float _ambient_illumination = 0.0f;
	float _attenuation_start = 1.0f;
	float _attenuation_end = 100.0f;
	bool _rectangle_shape = false;
	float _aspect_ratio = 1.0f;

	bool _shadow_caster = false;
	bool _light_caster = true;
	SceneLightPtr _shadow_source;

	int shadow_map_index = -1;

	uicore::Mat4f shadow_to_projection(uicore::ClipZRange clip_z_range) const
	{
		using namespace uicore;
		if (type() == SceneLight::type_spot)
		{
			float field_of_view = falloff();
			return Mat4f::perspective(field_of_view, aspect_ratio(), 0.1f, 1.e10f, handed_left, clip_z_range);
		}
		else if (type() == SceneLight::type_directional)
		{
			return Mat4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1.e10f, handed_left, clip_z_range);
		}
		else
		{
			return Mat4f::identity();
		}
	}

	uicore::Mat4f world_to_shadow() const
	{
		using namespace uicore;
		Quaternionf inv_orientation = Quaternionf::inverse(orientation());
		return inv_orientation.to_matrix() * Mat4f::translate(-position());
	}

	uicore::Mat4f world_to_shadow_projection(uicore::ClipZRange clip_z_range) const
	{
		return shadow_to_projection(clip_z_range) * world_to_shadow();
	}
};
