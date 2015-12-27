
#pragma once

#include "player_pawn.h"
#include "Model/Network/NetGame/event.h"

class SpawnPoint;

class ServerPlayerPawn : public PlayerPawn
{
public:
	ServerPlayerPawn(GameWorld *world, const std::string &owner, std::shared_ptr<SpawnPoint> spawn);
	~ServerPlayerPawn();

	void tick(const GameTick &tick) override;
	void net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event) override;

	void send_net_create(const GameTick &tick, const std::string &target);
	void send_net_update(const GameTick &tick);

	std::string owner;

	void apply_damage(const GameTick &tick, float damage) override;

private:
	void add_update_args(uicore::NetGameEvent &net_event, const GameTick &tick, bool is_owner);
};
