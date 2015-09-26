
#pragma once

#include "player_pawn.h"
#include "Model/Network/NetGame/event.h"

class ClientPlayerPawn : public PlayerPawn
{
public:
	ClientPlayerPawn(GameWorld *world);
	~ClientPlayerPawn();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

	void net_create(const GameTick &tick, const uicore::NetGameEvent &net_event);
	void net_update(const GameTick &tick, const uicore::NetGameEvent &net_event);
	void net_hit(const GameTick &tick, const uicore::NetGameEvent &net_event);

	SceneCamera camera;

	bool get_is_owner() const { return is_owner; }

private:
	bool is_owner = false;

	SceneObject scene_object;
	AudioObject sound;
	float step_movement = 0.0f;
	bool left_step = false;

	std::string last_anim = "default";

	Physics3DSweepTest camera_sweep_test;
	Physics3DShape camera_shape;

	uicore::Vec3f last_position;
	uicore::Vec3f next_position;

	float zoom_out = 0.0f;

	std::vector<PlayerPawnMovement> sent_movements;
};
