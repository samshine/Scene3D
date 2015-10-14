
#pragma once

#include <memory>

class Scene;
class SceneModel;
typedef std::shared_ptr<SceneModel> SceneModelPtr;
class SceneObject_Impl;

class SceneObject
{
public:
	SceneObject();
	SceneObject(Scene &scene, const SceneModelPtr &model, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf(), const uicore::Vec3f &scale = uicore::Vec3f(1.0f));
	bool is_null() const { return !impl; }

	uicore::Vec3f get_position() const;
	uicore::Quaternionf get_orientation() const;
	uicore::Vec3f get_scale() const;
	void set_position(const uicore::Vec3f &position);
	void set_orientation(const uicore::Quaternionf &orientation);
	void set_scale(const uicore::Vec3f &scale);

	void set_light_probe_receiver(bool enable);

	// \brief Moves object relative to its current orientation
	SceneObject &move(uicore::Vec3f offset);

	// \brief Rotate object relative to its current orientation
	SceneObject &rotate(float dir, float up, float tilt);

	// \brief Returns the currently active animation
	std::string get_animation() const;

	// \brief Plays a new animation
	// \param instant True if animation should change immidately. False if it should wait for current to finish
	void play_animation(const std::string &name, bool instant);

	// \brief Plays two sequential animations
	// \param instant True if animation should change immidately. False if it should wait for current to finish
	void play_transition(const std::string &anim1, const std::string &anim2, bool instant);

	// \brief Updates current animation playback
	void update(float time_elapsed);

	// \brief Updates current animation based on movement
	void moved(float units_moved);

	// \brief Returns the current position of an object attachment
	void get_attachment_location(const std::string &name, uicore::Vec3f &position, uicore::Quaternionf &orientation, uicore::Vec3f &scale) const;

private:
	std::shared_ptr<SceneObject_Impl> impl;
};
