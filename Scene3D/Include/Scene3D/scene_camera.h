
#pragma once

#include <memory>

class Scene;

class SceneCamera
{
public:
	static std::shared_ptr<SceneCamera> create(Scene &scene);

	virtual uicore::Vec3f position() const = 0;
	virtual uicore::Quaternionf orientation() const = 0;

	virtual float field_of_view() const = 0;

	virtual void set_position(const uicore::Vec3f &position) = 0;
	virtual void set_orientation(const uicore::Quaternionf &orientation) = 0;

	virtual void set_field_of_view(float fov) = 0;
};

typedef std::shared_ptr<SceneCamera> SceneCameraPtr;
