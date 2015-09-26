
#pragma once

#include "game_object.h"

class Bullet : public GameObject
{
public:
	Bullet(GameWorld *world, const std::string &type, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation);
	~Bullet();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

private:
	float time_left = 0.0f;

	uicore::Vec3f pos;
	uicore::Quaternionf orientation;
	uicore::Vec3f velocity;

	uicore::Vec3f last_pos;
	uicore::Quaternionf last_orientation;

	uicore::Physics3DRayTest ray_test;

	uicore::SceneObject scene_object;
	uicore::SceneParticleEmitter emitter;
	AudioObject sound;

	float gravity = 0.0f;
	bool bounce = false;
	float damage = 0.0f;

	std::string explosion_type;
};
