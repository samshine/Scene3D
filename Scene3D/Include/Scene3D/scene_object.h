
#pragma once

#include <memory>

class Scene;
class SceneModel;
typedef std::shared_ptr<SceneModel> SceneModelPtr;

class SceneObject
{
public:
	static std::shared_ptr<SceneObject> create(Scene &scene, const SceneModelPtr &model, const uicore::Vec3f &position = uicore::Vec3f(0.0f), const uicore::Quaternionf &orientation = uicore::Quaternionf(), const uicore::Vec3f &scale = uicore::Vec3f(1.0f));

	virtual uicore::Vec3f position() const = 0;
	virtual uicore::Quaternionf orientation() const = 0;
	virtual uicore::Vec3f scale() const = 0;
	virtual void set_position(const uicore::Vec3f &position) = 0;
	virtual void set_orientation(const uicore::Quaternionf &orientation) = 0;
	virtual void set_scale(const uicore::Vec3f &scale) = 0;

	virtual bool light_probe_receiver() const = 0;
	virtual void set_light_probe_receiver(bool enable) = 0;

	// \brief Moves object relative to its current orientation
	virtual void move(uicore::Vec3f offset) = 0;

	// \brief Rotate object relative to its current orientation
	virtual void rotate(float dir, float up, float tilt) = 0;

	// \brief Returns the currently active animation
	virtual std::string animation() const = 0;

	// \brief Plays a new animation
	// \param instant True if animation should change immidately. False if it should wait for current to finish
	virtual void play_animation(const std::string &name, bool instant) = 0;

	// \brief Plays two sequential animations
	// \param instant True if animation should change immidately. False if it should wait for current to finish
	virtual void play_transition(const std::string &anim1, const std::string &anim2, bool instant) = 0;

	// \brief Updates current animation playback
	virtual void update(float time_elapsed) = 0;

	// \brief Updates current animation based on movement
	virtual void moved(float units_moved) = 0;

	// \brief Returns the current position of an object attachment
	virtual void attachment_location(const std::string &name, uicore::Vec3f &position, uicore::Quaternionf &orientation, uicore::Vec3f &scale) const = 0;
};

typedef std::shared_ptr<SceneObject> SceneObjectPtr;
