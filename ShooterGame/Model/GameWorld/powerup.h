
#pragma once

#include "game_object.h"

class Powerup : public GameObject
{
public:
	Powerup(GameWorld *world, const clan::Vec3f &pos, const clan::Quaternionf &orientation, const std::string &model_name, float scale, const std::string &animation, const clan::Vec3f &collision_box_size, float respawn_time, const std::string &powerup_type);
	~Powerup();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

protected:
	clan::SceneObject scene_object;
};
