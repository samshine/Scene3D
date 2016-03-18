
#include "precomp.h"
#include "server_player_pawn.h"
#include "spawn_point.h"
#include "game_master.h"
#include <algorithm>

using namespace uicore;

ServerPlayerPawn::ServerPlayerPawn(GameWorld *world, const std::string &owner, std::shared_ptr<SpawnPoint> spawn) : PlayerPawn(world), owner(owner)
{
	dir = spawn->dir;
	up = spawn->up;
	character_controller.warp(spawn->pos, Vec3f(), false);

	func_received_event("player-pawn-input") = bind_member(this, &ServerPlayerPawn::on_player_pawn_input);
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

void ServerPlayerPawn::on_player_pawn_input(const std::string &sender, const uicore::JsonValue &message)
{
	if (sender != owner)
		return;

	key_forward.next_pressed = message["forward"].to_boolean();
	key_back.next_pressed = message["back"].to_boolean();
	key_left.next_pressed = message["left"].to_boolean();
	key_right.next_pressed = message["right"].to_boolean();
	key_jump.next_pressed = message["jump"].to_boolean();
	key_fire_primary.next_pressed = message["firePrimary"].to_boolean();
	key_fire_secondary.next_pressed = message["fireSecondary"].to_boolean();
	key_weapon = message["weapon"].to_int();
	dir = message["dir"].to_float();
	up = message["up"].to_float();
}

void ServerPlayerPawn::apply_damage(float damage)
{
	float last_health = health;

	health -= damage;

	auto message = JsonValue::object();
	message["name"] = JsonValue::string("player-pawn-hit");
	send_event("all", message);

	if (health <= 0.0f && last_health > 0.0f)
	{
		GameMaster::instance(this)->player_killed(cast<ServerPlayerPawn>());
	}
}

void ServerPlayerPawn::apply_impulse(const uicore::Vec3f &force)
{
	character_controller.apply_impulse(force);
}

void ServerPlayerPawn::send_net_create(const std::string &target)
{
	auto net_event = JsonValue::object();
	net_event["name"] = JsonValue::string("create");
	net_event["type"] = JsonValue::string("player-pawn");
	add_update_args(net_event, false);

	auto net_event_owner = JsonValue::object();
	net_event_owner["name"] = JsonValue::string("create");
	net_event_owner["type"] = JsonValue::string("player-pawn");
	add_update_args(net_event_owner, true);

	if (target == "all" && owner != "server")
	{
		send_event("!" + owner, net_event);
		send_event(owner, net_event_owner);
	}
	else
	{
		send_event(target, target == owner ? net_event_owner : net_event);
	}
}

void ServerPlayerPawn::send_net_update()
{
	auto net_event = JsonValue::object();
	net_event["name"] = JsonValue::string("player-pawn-update");
	net_event["type"] = JsonValue::string("player-pawn");
	add_update_args(net_event, false);

	auto net_event_owner = JsonValue::object();
	net_event_owner["name"] = JsonValue::string("player-pawn-update");
	net_event_owner["type"] = JsonValue::string("player-pawn");
	add_update_args(net_event_owner, true);

	if (owner != "server")
	{
		send_event("!" + owner, net_event);
		send_event(owner, net_event_owner);
	}
	else
	{
		send_event("all", net_event);
	}
}

void ServerPlayerPawn::add_update_args(uicore::JsonValue &message, bool is_owner)
{
	Vec3f pos = character_controller.get_position();
	Vec3f velocity = character_controller.get_velocity();
	bool is_flying = character_controller.is_flying();

	message["owner"] = JsonValue::boolean(is_owner);
	message["forward"] = JsonValue::boolean(key_forward.next_pressed);
	message["back"] = JsonValue::boolean(key_back.next_pressed);
	message["left"] = JsonValue::boolean(key_left.next_pressed);
	message["right"] = JsonValue::boolean(key_right.next_pressed);
	message["jump"] = JsonValue::boolean(key_jump.next_pressed);
	message["firePrimary"] = JsonValue::boolean(key_fire_primary.next_pressed);
	message["fireSecondary"] = JsonValue::boolean(key_fire_secondary.next_pressed);
	message["weapon"] = JsonValue::number(key_weapon);
	message["dir"] = JsonValue::number(dir);
	message["up"] = JsonValue::number(up);
	message["x"] = JsonValue::number(pos.x);
	message["y"] = JsonValue::number(pos.y);
	message["z"] = JsonValue::number(pos.z);
	message["vx"] = JsonValue::number(velocity.x);
	message["vy"] = JsonValue::number(velocity.y);
	message["vz"] = JsonValue::number(velocity.z);
	message["flying"] = JsonValue::boolean(is_flying);
	message["health"] = JsonValue::number(health);
}
