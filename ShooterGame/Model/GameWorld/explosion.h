
#pragma once

#include "game_object.h"

class Explosion : public GameObject
{
public:
	Explosion(GameWorld *world, const std::string &type, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation);
	~Explosion();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

protected:
	uicore::Vec3f pos;
	uicore::Quaternionf orientation;
	SceneObjectPtr scene_object;
	SceneParticleEmitterPtr emitter;
	AudioObjectPtr sound;

	float time_left = 0.0f;
};
