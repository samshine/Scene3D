
#pragma once

#include "Scene3D/scene_decal.h"
#include "Scene3D/Culling/scene_cull_provider.h"
#include <list>

class SceneImpl;

class SceneDecalImpl : public SceneDecal, public SceneItem
{
public:
	SceneDecalImpl(SceneImpl *scene);
	~SceneDecalImpl();

	uicore::Vec3f position() const override { return _position; }
	uicore::Quaternionf orientation() const override { return _orientation; }
	uicore::Vec3f extents() const override { return _extents; }
	float cutoff_angle() const override { return _cutoff_angle; }
	std::string diffuse_texture() const override { return _diffuse_texture; }
	std::string normal_texture() const override { return _normal_texture; }
	std::string specular_texture() const override { return _specular_texture; }
	std::string self_illumination_texture() const override { return _self_illumination_texture; }
	float glossiness() const override { return _glossiness; }
	float specular_level() const override { return _specular_level; }

	void set_position(const uicore::Vec3f &position) override;
	void set_orientation(const uicore::Quaternionf &orientation) override;
	void set_extents(const uicore::Vec3f &extents) override;
	void set_cutoff_angle(float value) override { _cutoff_angle = value; }
	void set_diffuse_texture(const std::string &texture) override { _diffuse_texture = texture; }
	void set_normal_texture(const std::string &texture) override { _normal_texture = texture; }
	void set_specular_texture(const std::string &texture) override { _specular_texture = texture; }
	void set_self_illumination_texture(const std::string &texture) override { _self_illumination_texture = texture; }
	void set_glossiness(float value) override { _glossiness = value; }
	void set_specular_level(float value) override { _specular_level = value; }

	uicore::AxisAlignedBoundingBox get_aabb();

	SceneImpl *scene = nullptr;
	SceneCullProxy *cull_proxy = nullptr;
	std::list<SceneDecalImpl *>::iterator it;

	uicore::Vec3f _position;
	uicore::Quaternionf _orientation;
	uicore::Vec3f _extents;
	float _cutoff_angle = 60.0f;
	std::string _diffuse_texture;
	std::string _normal_texture;
	std::string _specular_texture;
	std::string _self_illumination_texture;
	float _glossiness = 0.0f;
	float _specular_level = 0.0f;
};
