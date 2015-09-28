
#pragma once

class MapDescObject
{
public:
	std::string id;
	std::string type;
	uicore::Vec3f position;
	float dir = 0.0f;
	float up = 0.0f;
	float tilt = 0.0f;
	float scale = 1.0f;
	std::string mesh;
	std::string animation = "default";
	uicore::JsonValue fields = uicore::JsonValue::object();
};
