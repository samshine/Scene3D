
#include "precomp.h"
#include "server_player_pawn.h"
#include "game_tick.h"
#include "game_world.h"
#include "spawn_point.h"
#include "game_master.h"
#include <algorithm>

using namespace uicore;

ServerPlayerPawn::ServerPlayerPawn(GameWorld *world, const std::string &owner, std::shared_ptr<SpawnPoint> spawn) : PlayerPawn(world), owner(owner)
{
	dir = spawn->dir;
	up = spawn->up;
	character_controller.warp(spawn->pos, Vec3f(), false);
}

ServerPlayerPawn::~ServerPlayerPawn()
{
}

void ServerPlayerPawn::tick()
{
	PlayerPawn::tick();
	send_net_update();
	/*
	static FilePtr file = File::create_always("c:\\development\\debug_server_player.csv");
	auto str = string_format("%1;%2;%3;%4;%5;", receive_tick_time(), arrival_tick_time(), character_controller.get_position().x, character_controller.get_position().y, character_controller.get_position().z);
	str += string_format("%1;%2;%3;%4\r\n", character_controller.get_velocity().x, character_controller.get_velocity().y, character_controller.get_velocity().z, character_controller.is_flying());
	file->write(str.data(), str.length());
	*/
}

void ServerPlayerPawn::net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event)
{
	if (net_event.get_name() == "player-pawn-input" && sender == owner)
	{
		key_forward.next_pressed = net_event.get_argument(1).get_boolean();
		key_back.next_pressed = net_event.get_argument(2).get_boolean();
		key_left.next_pressed = net_event.get_argument(3).get_boolean();
		key_right.next_pressed = net_event.get_argument(4).get_boolean();
		key_jump.next_pressed = net_event.get_argument(5).get_boolean();
		key_fire_primary.next_pressed = net_event.get_argument(6).get_boolean();
		key_fire_secondary.next_pressed = net_event.get_argument(7).get_boolean();
		key_weapon = net_event.get_argument(8).get_integer();
		dir = net_event.get_argument(9).get_number();
		up = net_event.get_argument(10).get_number();
	}
}

void ServerPlayerPawn::apply_damage(float damage)
{
	float last_health = health;

	health -= damage;

	NetGameEvent net_event("player-pawn-hit");
	net_event.add_argument(id());

	send_net_event("all", net_event);

	if (health <= 0.0f && last_health > 0.0f)
	{
		if (world()->game_master)
			world()->game_master->player_killed(to_type<ServerPlayerPawn>());
	}
}

void ServerPlayerPawn::apply_impulse(const uicore::Vec3f &force)
{
	character_controller.apply_impulse(force);
}

void ServerPlayerPawn::send_net_create(const std::string &target)
{
	NetGameEvent net_event("create");
	NetGameEvent net_event_owner("create");

	net_event.add_argument(id());
	net_event.add_argument("player-pawn");
	net_event_owner.add_argument(id());
	net_event_owner.add_argument("player-pawn");

	add_update_args(net_event, false);
	add_update_args(net_event_owner, true);

	if (target == "all" && owner != "server")
	{
		send_net_event("!" + owner, net_event);
		send_net_event(owner, net_event_owner);
	}
	else
	{
		send_net_event(target, target == owner ? net_event_owner : net_event);
	}
}

void ServerPlayerPawn::send_net_update()
{
	NetGameEvent net_event("player-pawn-update");
	NetGameEvent net_event_owner("player-pawn-update");

	net_event.add_argument(id());
	net_event_owner.add_argument(id());

	add_update_args(net_event, false);
	add_update_args(net_event_owner, true);

	if (owner != "server")
	{
		send_net_event("!" + owner, net_event);
		send_net_event(owner, net_event_owner);
	}
	else
	{
		send_net_event("all", net_event);
	}
}

void ServerPlayerPawn::add_update_args(uicore::NetGameEvent &net_event, bool is_owner)
{
	Vec3f pos = character_controller.get_position();
	Vec3f velocity = character_controller.get_velocity();
	bool is_flying = character_controller.is_flying();

	net_event.add_argument(NetGameEventValue(is_owner));
	net_event.add_argument(NetGameEventValue(key_forward.next_pressed));
	net_event.add_argument(NetGameEventValue(key_back.next_pressed));
	net_event.add_argument(NetGameEventValue(key_left.next_pressed));
	net_event.add_argument(NetGameEventValue(key_right.next_pressed));
	net_event.add_argument(NetGameEventValue(key_jump.next_pressed));
	net_event.add_argument(NetGameEventValue(key_fire_primary.next_pressed));
	net_event.add_argument(NetGameEventValue(key_fire_secondary.next_pressed));
	net_event.add_argument(key_weapon);
	net_event.add_argument(dir);
	net_event.add_argument(up);
	net_event.add_argument(pos.x);
	net_event.add_argument(pos.y);
	net_event.add_argument(pos.z);
	net_event.add_argument(velocity.x);
	net_event.add_argument(velocity.y);
	net_event.add_argument(velocity.z);
	net_event.add_argument(NetGameEventValue(is_flying));
	net_event.add_argument(health);
}
