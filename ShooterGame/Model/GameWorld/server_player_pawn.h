
#pragma once

#include "player_pawn.h"
#include "Model/Network/NetGame/event.h"

class SpawnPoint;

class ServerPlayerPawn : public PlayerPawn
{
public:
	ServerPlayerPawn(GameWorld *world, const std::string &owner, std::shared_ptr<SpawnPoint> spawn);
	~ServerPlayerPawn();

	void tick() override;
	void net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event) override;

	void send_net_create(const std::string &target);
	void send_net_update();

	std::string owner;

	void apply_damage(float damage) override;
	void apply_impulse(const uicore::Vec3f &force) override;

private:
	void add_update_args(uicore::NetGameEvent &net_event, bool is_owner);
};
