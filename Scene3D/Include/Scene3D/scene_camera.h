
#pragma once

#include <memory>

class Scene;
class SceneCamera_Impl;

class SceneCamera
{
public:
	SceneCamera();
	SceneCamera(Scene &scene);

	bool is_null() const;

	uicore::Vec3f get_position() const;
	uicore::Quaternionf get_orientation() const;

	float get_field_of_view() const;

	void set_position(const uicore::Vec3f &position);
	void set_orientation(const uicore::Quaternionf &orientation);

	void set_field_of_view(float fov);

private:
	std::shared_ptr<SceneCamera_Impl> impl;

	friend class Scene;
	friend class Scene_Impl;
};
