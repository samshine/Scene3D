
#pragma once

#include "server_player_pawn.h"

enum class RobotPlayerMode
{
	stationary,
	chase,
	path
};

class RobotPlayerPawn : public ServerPlayerPawn
{
public:
	RobotPlayerPawn(const std::string &owner, std::shared_ptr<SpawnPoint> spawn);

	void tick() override;
	void apply_impulse(const uicore::Vec3f &force) override;

private:
	void tick_stationary();
	void tick_chase();
	void tick_path();

	void track_target();

	std::vector<int> create_path_steps(int start_segment, int end_segment, int max_depth, int avoid_segment);

	std::vector<int> current_path;
	size_t current_path_index = 0;

	uicore::Vec3f last_seen_target_pos;
	float mode_change_cooldown = 0.0f;
	float weapon_change_cooldown = 15.0f;

	float aim_angle_error = 0.0f;
	float aim_angle_cooldown = 0.0f;

	RobotPlayerMode mode = RobotPlayerMode::stationary;
};
