
#pragma once

#include "server_player_pawn.h"

enum class RobotPlayerMode
{
	idle,
	follow
};

class RobotPlayerPawn : public ServerPlayerPawn
{
public:
	RobotPlayerPawn(GameWorld *world, const std::string &owner, std::shared_ptr<SpawnPoint> spawn);

	void tick(const GameTick &tick) override;

private:
	void tick_idle(const GameTick &tick);
	void tick_follow(const GameTick &tick);

	uicore::Vec3f last_seen_target_pos;
	float move_cooldown = 20.0f;
	float weapon_change_cooldown = 15.0f;

	RobotPlayerMode mode = RobotPlayerMode::follow;
};
