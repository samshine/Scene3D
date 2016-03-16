
#pragma once

class PlayerPawn;

class Rocket : public GameObject
{
public:
	Rocket(GameWorld *world, PlayerPawn *owner, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation);
	Rocket(GameWorld *world, const uicore::JsonValue &net_event);

	void send_create();

	void tick() override;
	void frame() override;

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
