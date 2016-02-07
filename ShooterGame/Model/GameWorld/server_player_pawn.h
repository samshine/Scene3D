
#pragma once

#include "player_pawn.h"

class SpawnPoint;

class ServerPlayerPawn : public PlayerPawn
{
public:
	ServerPlayerPawn(const std::string &owner, std::shared_ptr<SpawnPoint> spawn);
	~ServerPlayerPawn();

	void tick() override;

	void send_net_create(const std::string &target);
	void send_net_update();

	std::string owner;

	void apply_damage(float damage) override;
	void apply_impulse(const uicore::Vec3f &force) override;

private:
	void on_player_pawn_input(const std::string &sender, const uicore::JsonValue &message);
	void add_update_args(uicore::JsonValue &message, bool is_owner);
};
