
#pragma once

#include "game_object.h"

class SpawnPoint : public GameObject
{
public:
	SpawnPoint(GameWorld *world, const clan::Vec3f &pos, float dir, float up, float tilt, const std::string &team);

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

	clan::Vec3f pos;
	float dir;
	float up;
	float tilt;
	std::string team;
};
