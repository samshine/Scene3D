
#pragma once

#include "game_object.h"

class Bullet : public GameObject
{
public:
	Bullet(GameWorld *world, const std::string &type, const clan::Vec3f &pos, const clan::Quaternionf &orientation);
	~Bullet();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

private:
	float time_left = 0.0f;

	clan::Vec3f pos;
	clan::Quaternionf orientation;
	clan::Vec3f velocity;

	clan::Vec3f last_pos;
	clan::Quaternionf last_orientation;

	clan::Physics3DRayTest ray_test;

	clan::SceneObject scene_object;
	clan::SceneParticleEmitter emitter;
	clan::AudioObject sound;

	float gravity = 0.0f;
	bool bounce = false;
	float damage = 0.0f;

	std::string explosion_type;
};
