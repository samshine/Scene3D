
#include "precomp.h"
#include "server_player_pawn.h"
#include "game_tick.h"
#include "game_world.h"
#include "spawn_point.h"
#include <algorithm>

using namespace uicore;

ServerPlayerPawn::ServerPlayerPawn(GameWorld *world, const std::string &owner, SpawnPoint *spawn) : PlayerPawn(world), owner(owner)
{
	cur_movement.dir = spawn->dir;
	cur_movement.up = spawn->up;
	character_controller.warp(spawn->pos, Vec3f(), false);
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

	world()->network->queue_event("all", net_event, tick.arrival_tick_time);

	if (health <= 0.0f && last_health > 0.0f)
	{
		world()->player_killed(tick, this);
	}
}

void ServerPlayerPawn::net_input(const GameTick &tick, const uicore::NetGameEvent &net_event)
{
	cur_movement.key_forward.next_pressed = net_event.get_argument(0).get_boolean();
	cur_movement.key_back.next_pressed = net_event.get_argument(1).get_boolean();
	cur_movement.key_left.next_pressed = net_event.get_argument(2).get_boolean();
	cur_movement.key_right.next_pressed = net_event.get_argument(3).get_boolean();
	cur_movement.key_jump.next_pressed = net_event.get_argument(4).get_boolean();
	cur_movement.key_fire_primary.next_pressed = net_event.get_argument(5).get_boolean();
	cur_movement.key_fire_secondary.next_pressed = net_event.get_argument(6).get_boolean();
	cur_movement.key_weapon = net_event.get_argument(7).get_integer();
	cur_movement.dir = net_event.get_argument(8).get_number();
	cur_movement.up = net_event.get_argument(9).get_number();
}

void ServerPlayerPawn::send_net_create(const GameTick &tick, const std::string &target)
{
	NetGameEvent net_event("player-pawn-create");
	NetGameEvent net_event_owner("player-pawn-create");

	add_update_args(net_event, tick, false);
	add_update_args(net_event_owner, tick, true);

	if (target == "all" && owner != "server")
	{
		world()->network->queue_event("!" + owner, net_event, tick.arrival_tick_time);
		world()->network->queue_event(owner, net_event_owner, tick.arrival_tick_time);
	}
	else
	{
		world()->network->queue_event(target, target == owner ? net_event_owner : net_event, tick.arrival_tick_time);
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
		world()->network->queue_event("!" + owner, net_event, tick.arrival_tick_time);
		world()->network->queue_event(owner, net_event_owner, tick.arrival_tick_time);
	}
	else
	{
		world()->network->queue_event("all", net_event, tick.arrival_tick_time);
	}
}

void ServerPlayerPawn::send_net_destroy(const GameTick &tick)
{
	NetGameEvent net_event("player-pawn-destroy");
	net_event.add_argument(id());

	world()->network->queue_event("all", net_event, tick.arrival_tick_time);
}

void ServerPlayerPawn::add_update_args(uicore::NetGameEvent &net_event, const GameTick &tick, bool is_owner)
{
	Vec3f pos = character_controller.get_position();
	Vec3f velocity = character_controller.get_velocity();
	bool is_flying = character_controller.is_flying();

	net_event.add_argument(id());
	net_event.add_argument(NetGameEventValue(is_owner));
	net_event.add_argument(NetGameEventValue(cur_movement.key_forward.next_pressed));
	net_event.add_argument(NetGameEventValue(cur_movement.key_back.next_pressed));
	net_event.add_argument(NetGameEventValue(cur_movement.key_left.next_pressed));
	net_event.add_argument(NetGameEventValue(cur_movement.key_right.next_pressed));
	net_event.add_argument(NetGameEventValue(cur_movement.key_jump.next_pressed));
	net_event.add_argument(NetGameEventValue(cur_movement.key_fire_primary.next_pressed));
	net_event.add_argument(NetGameEventValue(cur_movement.key_fire_secondary.next_pressed));
	net_event.add_argument(cur_movement.key_weapon);
	net_event.add_argument(cur_movement.dir);
	net_event.add_argument(cur_movement.up);
	net_event.add_argument(pos.x);
	net_event.add_argument(pos.y);
	net_event.add_argument(pos.z);
	net_event.add_argument(velocity.x);
	net_event.add_argument(velocity.y);
	net_event.add_argument(velocity.z);
	net_event.add_argument(NetGameEventValue(is_flying));
	net_event.add_argument(health);
}
