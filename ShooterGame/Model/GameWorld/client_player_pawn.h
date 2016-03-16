
#pragma once

#include "player_pawn.h"

class ClientPlayerPawn : public PlayerPawn
{
public:
	ClientPlayerPawn(GameWorld *world);
	~ClientPlayerPawn();

	void tick() override;
	void frame() override;

	void net_create(const uicore::JsonValue &net_event);
	void net_update(const uicore::JsonValue &net_event);
	void net_hit(const uicore::JsonValue &net_event);

	SceneCameraPtr camera;

	bool get_is_owner() const { return is_owner; }

private:
	void shake_camera(float magnitude, float duration);

	bool is_owner = false;

	SceneObjectPtr scene_object;
	AudioObjectPtr sound;
	float step_movement = 0.0f;
	bool left_step = false;

	std::string last_anim = "default";

	Physics3DShapePtr camera_shape;

	uicore::Vec3f last_position;
	uicore::Vec3f next_position;

	float zoom_out = 0.0f;

	std::vector<PlayerPawnMovement> sent_movements;

	float shake_magnitude = 0.0f;
	float shake_timer = 0.0f;
	float shake_speed = 4.0f;
};
