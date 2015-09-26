
#pragma once

class LevelDataLight
{
public:
	LevelDataLight() { }
	LevelDataLight(const uicore::Vec3f &position, const uicore::Quaternionf &orientation, const uicore::Vec3f &color, float attenuation_start, float attenuation_end, float falloff, float hotspot, bool casts_shadows)
		: position(position), orientation(orientation), color(color), attenuation_start(attenuation_start), attenuation_end(attenuation_end), falloff(falloff), hotspot(hotspot), casts_shadows(casts_shadows), level_set(0) { }

	uicore::Vec3f position;
	uicore::Quaternionf orientation;
	uicore::Vec3f color;
	float attenuation_start = 0.0f;
	float attenuation_end = 0.0f;
	float falloff = 0.0f;
	float hotspot = 0.0f;
	bool casts_shadows = false;
	int level_set = 0;
};
