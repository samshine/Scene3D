
#include "precomp.h"
#include "server_player_pawn.h"
#include "game_tick.h"
#include "game_world.h"
#include "spawn_point.h"
#include "Model/game.h"
#include <algorithm>

using namespace clan;

ServerPlayerPawn::ServerPlayerPawn(GameWorld *world, const std::string &owner, SpawnPoint *spawn) : PlayerPawn(world), owner(owner)
{
	controller->set_position(spawn->pos);
	dir = spawn->dir;
	up = spawn->up;
	tilt = spawn->tilt;
}

ServerPlayerPawn::~ServerPlayerPawn()
{
}

void ServerPlayerPawn::tick(const GameTick &tick)
{
	send_net_update(tick);

	PlayerPawn::tick(tick);
}

void ServerPlayerPawn::apply_damage(const GameTick &tick, float damage)
{
	float last_health = health;

	health -= damage;

	NetGameEvent net_event("player-pawn-hit");
	net_event.add_argument(id());

	world()->game()->network->queue_event("all", net_event, tick.arrival_tick_time);

	if (health <= 0.0f && last_health > 0.0f)
	{
		world()->player_killed(tick, this);
	}
}

void ServerPlayerPawn::net_input(const GameTick &tick, const clan::NetGameEvent &net_event)
{
	key_forward = net_event.get_argument(0).get_boolean();
	key_back = net_event.get_argument(1).get_boolean();
	key_left = net_event.get_argument(2).get_boolean();
	key_right = net_event.get_argument(3).get_boolean();
	key_jump = net_event.get_argument(4).get_boolean();
	key_fire_primary = net_event.get_argument(5).get_boolean();
	key_fire_secondary = net_event.get_argument(6).get_boolean();
	key_weapon = net_event.get_argument(7).get_integer();
	dir = net_event.get_argument(8).get_number();
	up = net_event.get_argument(9).get_number();
}

void ServerPlayerPawn::send_net_create(const GameTick &tick, const std::string &target)
{
	NetGameEvent net_event("player-pawn-create");
	NetGameEvent net_event_owner("player-pawn-create");

	add_update_args(net_event, tick, false);
	add_update_args(net_event_owner, tick, true);

	if (target == "all" && owner != "server")
	{
		world()->game()->network->queue_event("!" + owner, net_event, tick.arrival_tick_time);
		world()->game()->network->queue_event(owner, net_event_owner, tick.arrival_tick_time);
	}
	else
	{
		world()->game()->network->queue_event(target, target == owner ? net_event_owner : net_event, tick.arrival_tick_time);
	}
}

void ServerPlayerPawn::send_net_update(const GameTick &tick)
{
	NetGameEvent net_event("player-pawn-update");
	NetGameEvent net_event_owner("player-pawn-update");

	add_update_args(net_event, tick, false);
	add_update_args(net_event_owner, tick, true);

	if (owner != "server")
	{
		world()->game()->network->queue_event("!" + owner, net_event, tick.arrival_tick_time);
		world()->game()->network->queue_event(owner, net_event_owner, tick.arrival_tick_time);
	}
	else
	{
		world()->game()->network->queue_event("all", net_event, tick.arrival_tick_time);
	}
}

void ServerPlayerPawn::send_net_destroy(const GameTick &tick)
{
	NetGameEvent net_event("player-pawn-destroy");
	net_event.add_argument(id());

	world()->game()->network->queue_event("all", net_event, tick.arrival_tick_time);
}

void ServerPlayerPawn::add_update_args(clan::NetGameEvent &net_event, const GameTick &tick, bool is_owner)
{
	Vec3f pos = controller->get_position();
	bool is_flying = false;
	Vec3f fly_velocity = controller->get_fly_velocity(is_flying);

	net_event.add_argument(id());
	net_event.add_argument(NetGameEventValue(is_owner));
	net_event.add_argument(NetGameEventValue(key_forward));
	net_event.add_argument(NetGameEventValue(key_back));
	net_event.add_argument(NetGameEventValue(key_left));
	net_event.add_argument(NetGameEventValue(key_right));
	net_event.add_argument(NetGameEventValue(key_jump));
	net_event.add_argument(NetGameEventValue(key_fire_primary));
	net_event.add_argument(NetGameEventValue(key_fire_secondary));
	net_event.add_argument(key_weapon);
	net_event.add_argument(dir);
	net_event.add_argument(up);
	net_event.add_argument(pos.x);
	net_event.add_argument(pos.y);
	net_event.add_argument(pos.z);
	net_event.add_argument(move_velocity.x);
	net_event.add_argument(move_velocity.y);
	net_event.add_argument(move_velocity.z);
	net_event.add_argument(fly_velocity.x);
	net_event.add_argument(fly_velocity.y);
	net_event.add_argument(fly_velocity.z);
	net_event.add_argument(NetGameEventValue(is_flying));
	net_event.add_argument(health);
}
