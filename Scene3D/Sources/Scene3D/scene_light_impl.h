
#pragma once

#include "Scene3D/scene_light.h"
#include "Scene3D/scene_cull_provider.h"
#include <list>

class VSMShadowMapPassLightData;
class Scene_Impl;

class SceneLight_Impl : public SceneLight, public SceneItem
{
public:
	SceneLight_Impl(Scene_Impl *scene);
	~SceneLight_Impl();

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

	Scene_Impl *scene = nullptr;
	SceneCullProxy *cull_proxy = nullptr;
	std::list<SceneLight_Impl *>::iterator it;

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

	std::unique_ptr<VSMShadowMapPassLightData> vsm_data;
};

class SceneLightVisitor
{
public:
	virtual void light(const uicore::GraphicContextPtr &gc, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, SceneLight_Impl *light) = 0;
};
