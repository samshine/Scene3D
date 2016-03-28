
#pragma once

class PlayerPawn;

class ProjectileHit : public GameObject
{
public:
	ProjectileHit(GameWorld *world, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation);

	void send_create();

	void tick() override;
	void frame() override;

private:
	float time_left = 0.5f;

	uicore::Vec3f pos;
	uicore::Quaternionf orientation;

	SceneObjectPtr scene_object;
	SceneLightPtr light;
	SceneParticleEmitterPtr emitter;
	AudioObjectPtr sound;
};
