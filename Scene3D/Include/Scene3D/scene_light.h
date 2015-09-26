
#pragma once

#include <memory>

class Scene;
class SceneLight_Impl;
class OctTreeObject;
class VSMShadowMapPassLightData;

class SceneLight
{
public:
	SceneLight();
	SceneLight(Scene &scene);
	bool is_null() const { return !impl; }

	enum Type
	{
		type_omni,
		type_directional,
		type_spot,
		type_sun // directional light with parallel split shadow maps
	};

	Type get_type() const;
	uicore::Vec3f get_position() const;
	uicore::Quaternionf get_orientation() const;

	uicore::Vec3f get_color() const;
	float get_falloff() const;
	float get_hotspot() const;
	float get_ambient_illumination() const;
	float get_attenuation_start() const;
	float get_attenuation_end() const;
	bool is_rectangle_shape() const;
	float get_aspect_ratio() const;

	bool is_shadow_caster() const;
	bool is_light_caster() const;
	SceneLight get_shadow_source() const;

	void set_type(Type type);
	void set_position(const uicore::Vec3f &position);
	void set_orientation(const uicore::Quaternionf &orientation);

	void set_color(uicore::Vec3f color);
	void set_falloff(float falloff);
	void set_hotspot(float hotspot);
	void set_ambient_illumination(float factor);
	void set_attenuation_start(float attenuation_start);
	void set_attenuation_end(float attenuation_end);
	void set_rectangle_shape(bool rectangle);
	void set_aspect_ratio(float aspect);

	void set_shadow_caster(bool casts_shadows);
	void set_light_caster(bool casts_light);
	void set_shadow_source(SceneLight light);

private:
	std::shared_ptr<SceneLight_Impl> impl;
};
