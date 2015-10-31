
#pragma once

#include "Scene3D/scene_object.h"
#include "Scene3D/scene_light.h"
#include "Scene3D/Culling/scene_cull_provider.h"
#include "Scene3D/SceneEngine/Model/model_instance.h"
#include <list>

class SceneImpl;

class SceneObjectImpl : public SceneObject, public SceneItem
{
public:
	SceneObjectImpl(SceneImpl *scene);
	~SceneObjectImpl();

	uicore::Vec3f position() const override { return _position; }
	uicore::Quaternionf orientation() const override { return _orientation; }
	uicore::Vec3f scale() const override { return _scale; }
	void set_position(const uicore::Vec3f &position) override;
	void set_orientation(const uicore::Quaternionf &orientation) override;
	void set_scale(const uicore::Vec3f &scale) override;
	bool light_probe_receiver() const override { return _light_probe_receiver; }
	void set_light_probe_receiver(bool enable) override;
	void move(uicore::Vec3f offset) override;
	void rotate(float dir, float up, float tilt) override;
	std::string animation() const override;
	void play_animation(const std::string &name, bool instant) override;
	void play_transition(const std::string &anim1, const std::string &anim2, bool instant) override;
	void update(float time_elapsed) override;
	void moved(float units_moved) override;
	void attachment_location(const std::string &name, uicore::Vec3f &position, uicore::Quaternionf &orientation, uicore::Vec3f &scale) const override;

	void set_bone_positions(const std::vector<uicore::Vec3f> &positions) override;
	void set_bone_orientations(const std::vector<uicore::Quaternionf> &orientations) override;


	void create_lights(const ScenePtr &scene_base);
	void update_lights();

	uicore::Mat4f get_object_to_world() const
	{
		return uicore::Mat4f::translate(_position) * _orientation.to_matrix() * uicore::Mat4f::scale(_scale);
	}

	uicore::AxisAlignedBoundingBox get_aabb() const;

	SceneImpl *scene;
	SceneCullProxy *cull_proxy = nullptr;
	std::list<SceneObjectImpl *>::iterator it;

	uicore::Vec3f _position;
	uicore::Quaternionf _orientation;
	uicore::Vec3f _scale = uicore::Vec3f(1.0f);

	bool _light_probe_receiver = false;

	ModelInstance instance;
	std::vector<SceneLightPtr> lights;
};
