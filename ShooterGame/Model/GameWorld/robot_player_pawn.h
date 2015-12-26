
#pragma once

#include "server_player_pawn.h"

enum class RobotPlayerMode
{
	idle,
	follow,
	path
};

class RobotPlayerPawn : public ServerPlayerPawn
{
public:
	RobotPlayerPawn(GameWorld *world, const std::string &owner, std::shared_ptr<SpawnPoint> spawn);

	void tick(const GameTick &tick) override;

private:
	void tick_idle(const GameTick &tick);
	void tick_follow(const GameTick &tick);
	void tick_path(const GameTick &tick);

	std::vector<int> create_path_steps(int start_segment, int end_segment, int max_depth, int avoid_segment);

	std::vector<int> current_path;
	size_t current_path_index = 0;

	uicore::Vec3f last_seen_target_pos;
	float move_cooldown = 20.0f;
	float weapon_change_cooldown = 15.0f;

	float aim_angle_error = 0.0f;
	float aim_angle_cooldown = 0.0f;

	RobotPlayerMode mode = RobotPlayerMode::path;
};
