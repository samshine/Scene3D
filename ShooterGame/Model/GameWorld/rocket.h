
#pragma once

#include "Model/ClientWorld/client_world.h"

class PlayerPawn;

class Rocket : public GameObject, public ClientObject
{
public:
	Rocket(PlayerPawn *owner, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation);
	Rocket(const uicore::JsonValue &net_event);

	void send_create();

	void tick() override;
	void frame(float time_elapsed, float interpolated_time) override;

private:
	float time_left = 10.0f;

	PlayerPawn *owner = nullptr;

	uicore::Vec3f pos;
	uicore::Quaternionf orientation;

	uicore::Vec3f last_pos;
	uicore::Quaternionf last_orientation;

	SceneObjectPtr scene_object;
	SceneParticleEmitterPtr emitter;
	AudioObjectPtr sound;
};
