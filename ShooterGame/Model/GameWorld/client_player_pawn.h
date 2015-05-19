
#pragma once

#include "player_pawn.h"

class PastMovement
{
public:
	int tick_time = 0;

	// Input state before move
	bool key_forward = false;
	bool key_back = false;
	bool key_left = false;
	bool key_right = false;
	bool key_jump = false;
	bool key_fire_primary = false;
	bool key_fire_secondary = false;
	int key_weapon = 0;
	float dir = 0.0f;
	float up = 0.0f;

	// Character controller state before move
	clan::Vec3f pos;
	clan::Vec3f move_velocity;
	clan::Vec3f fly_velocity;
	bool is_flying = false;
};

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

	clan::Physics3DSweepTest camera_sweep_test;
	clan::Physics3DShape camera_shape;

	clan::Vec3f last_position;
	clan::Vec3f next_position;

	float zoom_out = 0.0f;

	std::vector<PastMovement> sent_movements;
};
