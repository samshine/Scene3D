
#pragma once

#include "player_pawn.h"

class ClientPlayerPawn : public PlayerPawn
{
public:
	ClientPlayerPawn(GameWorld *world);
	~ClientPlayerPawn();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

	void net_create(const GameTick &tick, const clan::NetGameEvent &net_event);
	void net_update(const GameTick &tick, const clan::NetGameEvent &net_event);
	void net_hit(const GameTick &tick, const clan::NetGameEvent &net_event);

	clan::SceneCamera camera;

	bool get_is_owner() const { return is_owner; }

private:
	bool is_owner = false;

	clan::SceneObject scene_object;
	clan::AudioObject sound;
	float step_movement = 0.0f;
	bool left_step = false;

	std::string last_anim = "default";

	clan::Physics3DSweepTest camera_sweep_test;
	clan::Physics3DShape camera_shape;

	clan::Vec3f last_position;
	clan::Vec3f next_position;

	float zoom_out = 0.0f;

	std::vector<PlayerPawnMovement> sent_movements;
};
