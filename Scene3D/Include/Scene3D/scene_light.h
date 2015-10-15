
#pragma once

#include <memory>

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class SceneLight
{
public:
	static std::shared_ptr<SceneLight> create(const ScenePtr &scene);

	enum Type
	{
		type_omni,
		type_directional,
		type_spot,
		type_sun // directional light with parallel split shadow maps
	};

	virtual Type type() const = 0;
	virtual uicore::Vec3f position() const = 0;
	virtual uicore::Quaternionf orientation() const = 0;

	virtual uicore::Vec3f color() const = 0;
	virtual float falloff() const = 0;
	virtual float hotspot() const = 0;
	virtual float ambient_illumination() const = 0;
	virtual float attenuation_start() const = 0;
	virtual float attenuation_end() const = 0;
	virtual bool rectangle_shape() const = 0;
	virtual float aspect_ratio() const = 0;

	virtual bool shadow_caster() const = 0;
	virtual bool light_caster() const = 0;
	virtual std::shared_ptr<SceneLight> shadow_source() const = 0;

	virtual void set_type(Type type) = 0;
	virtual void set_position(const uicore::Vec3f &position) = 0;
	virtual void set_orientation(const uicore::Quaternionf &orientation) = 0;

	virtual void set_color(uicore::Vec3f color) = 0;
	virtual void set_falloff(float falloff) = 0;
	virtual void set_hotspot(float hotspot) = 0;
	virtual void set_ambient_illumination(float factor) = 0;
	virtual void set_attenuation_start(float attenuation_start) = 0;
	virtual void set_attenuation_end(float attenuation_end) = 0;
	virtual void set_rectangle_shape(bool rectangle) = 0;
	virtual void set_aspect_ratio(float aspect) = 0;

	virtual void set_shadow_caster(bool casts_shadows) = 0;
	virtual void set_light_caster(bool casts_light) = 0;
	virtual void set_shadow_source(const std::shared_ptr<SceneLight> &light) = 0;
};

typedef std::shared_ptr<SceneLight> SceneLightPtr;
