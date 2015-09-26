
#pragma once

#include <memory>

class Scene;
class SceneLightProbe_Impl;

class SceneLightProbe
{
public:
	SceneLightProbe();
	SceneLightProbe(Scene &scene);
	bool is_null() const { return !impl; }

	uicore::Vec3f get_position() const;
	float get_radius() const;
	uicore::Vec3f get_color() const;

	void set_position(const uicore::Vec3f &position);
	void set_radius(float radius);
	void set_color(const uicore::Vec3f &color);

private:
	std::shared_ptr<SceneLightProbe_Impl> impl;
};
