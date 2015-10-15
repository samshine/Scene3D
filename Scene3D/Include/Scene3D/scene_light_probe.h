
#pragma once

#include <memory>

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class SceneLightProbe
{
public:
	static std::shared_ptr<SceneLightProbe> create(const ScenePtr &scene);

	virtual uicore::Vec3f position() const = 0;
	virtual float radius() const = 0;
	virtual uicore::Vec3f color() const = 0;

	virtual void set_position(const uicore::Vec3f &position) = 0;
	virtual void set_radius(float radius) = 0;
	virtual void set_color(const uicore::Vec3f &color) = 0;
};

typedef std::shared_ptr<SceneLightProbe> SceneLightProbePtr;
