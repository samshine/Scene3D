
#pragma once

#include "game_object.h"

class AlarmLights : public GameObject
{
public:
	AlarmLights(GameWorld *world);
	~AlarmLights();

	void tick(const GameTick &tick);
	void frame(float time_elapsed, float interpolated_time);

	void play_group_animation(const std::string &group_id, const std::string &animation_name);

private:
	std::map<std::string, std::vector<uicore::SceneObject>> groups;
};
