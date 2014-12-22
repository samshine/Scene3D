/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/


#pragma once

#include <memory>

namespace clan
{
/// \addtogroup clanScene_Scene clanScene Scene
/// \{

class Scene;
class SceneModel;
class SceneObject_Impl;

class SceneObject
{
public:
	SceneObject();
	SceneObject(Scene &scene, const SceneModel &model, const Vec3f &position = Vec3f(0.0f), const Quaternionf &orientation = Quaternionf(), const Vec3f &scale = Vec3f(1.0f));
	bool is_null() const { return !impl; }

	Vec3f get_position() const;
	Quaternionf get_orientation() const;
	Vec3f get_scale() const;
	void set_position(const Vec3f &position);
	void set_orientation(const Quaternionf &orientation);
	void set_scale(const Vec3f &scale);

	void set_light_probe_receiver(bool enable);

	// \brief Moves object relative to its current orientation
	SceneObject &move(Vec3f offset);

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
	void get_attachment_location(const std::string &name, Vec3f &position, Quaternionf &orientation, Vec3f &scale) const;

private:
	std::shared_ptr<SceneObject_Impl> impl;
};

}

/// \}
