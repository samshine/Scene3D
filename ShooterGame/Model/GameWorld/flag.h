
#pragma once

#include "game_object.h"

class Flag : public GameObject
{
public:
	Flag(GameWorld *world, const clan::Vec3f &pos, const clan::Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const clan::Vec3f &collision_box_size, const std::string &team);
	~Flag();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

protected:
	clan::SceneObject scene_object;
};
