
#pragma once

#include "game_object.h"

class Explosion : public GameObject
{
public:
	Explosion(GameWorld *world, const std::string &type, const clan::Vec3f &pos, const clan::Quaternionf &orientation);
	~Explosion();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

protected:
	clan::Vec3f pos;
	clan::Quaternionf orientation;
	clan::SceneObject scene_object;
	clan::SceneParticleEmitter emitter;
	clan::AudioObject sound;

	float time_left = 0.0f;
};
