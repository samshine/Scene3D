
#pragma once

#include <memory>

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class SceneDecal
{
public:
	static std::shared_ptr<SceneDecal> create(const ScenePtr &scene);

	virtual uicore::Vec3f position() const = 0;
	virtual uicore::Quaternionf orientation() const = 0;
	virtual uicore::Vec3f extents() const = 0;
	virtual float cutoff_angle() const = 0;
	virtual std::string diffuse_texture() const = 0;
	virtual std::string normal_texture() const = 0;
	virtual std::string specular_texture() const = 0;
	virtual std::string self_illumination_texture() const = 0;
	virtual float glossiness() const = 0;
	virtual float specular_level() const = 0;

	virtual void set_position(const uicore::Vec3f &position) = 0;
	virtual void set_orientation(const uicore::Quaternionf &orientation) = 0;
	virtual void set_extents(const uicore::Vec3f &extents) = 0;
	virtual void set_cutoff_angle(float value) = 0;
	virtual void set_diffuse_texture(const std::string &texture) = 0;
	virtual void set_normal_texture(const std::string &texture) = 0;
	virtual void set_specular_texture(const std::string &texture) = 0;
	virtual void set_self_illumination_texture(const std::string &texture) = 0;
	virtual void set_glossiness(float value) = 0;
	virtual void set_specular_level(float value) = 0;
};

typedef std::shared_ptr<SceneDecal> SceneDecalPtr;
