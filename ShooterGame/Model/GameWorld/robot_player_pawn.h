
#pragma once

#include "server_player_pawn.h"

class RobotPlayerPawn : public ServerPlayerPawn
{
public:
	RobotPlayerPawn(GameWorld *world, const std::string &owner, SpawnPoint *spawn);

	void tick(const GameTick &tick) override;

private:
	uicore::Vec3f last_seen_target_pos;
	float move_cooldown = 20.0f;
	float weapon_change_cooldown = 15.0f;
};
