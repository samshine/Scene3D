
#pragma once

#include "game_object.h"

class Flag : public GameObject
{
public:
	Flag(GameWorld *world, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const uicore::Vec3f &collision_box_size, const std::string &team);
	~Flag();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

protected:
	uicore::SceneObject scene_object;
};
