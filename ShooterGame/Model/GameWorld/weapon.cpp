
#include "precomp.h"
#include "weapon.h"
#include "client_player_pawn.h"
#include "game_world.h"
#include "bullet.h"
#include "Model/game.h"
#include <algorithm>

using namespace uicore;

Weapon::Weapon(PlayerPawn *player) : player(player)
{
	try_switch_to_weapon("IceLauncher");
}

Weapon::~Weapon()
{
}

void Weapon::tick(const GameTick &tick)
{
	animation_timer = std::max(animation_timer - tick.time_elapsed, 0.0f);

	switch (weapon_state)
	{
	case ready:
		tick_ready(tick);
		break;
	case hiding_old_weapon:
		tick_hiding_old_weapon(tick);
		break;
	case showing_new_weapon:
		tick_showing_new_weapon(tick);
		break;
	case firing:
		tick_firing(tick);
		break;
	}
}

void Weapon::start_fire(const std::string &subtype)
{
    if (weapon_state == ready && !weapon_type.empty())
	{
        weapon_subtype = subtype;
        should_stop_fire = false;

        JsonValue subtype_description = player->world()->weapon_data["weapons"][weapon_type][weapon_subtype];

        weapon_state = firing;
		animation_timer = subtype_description["cooldown"].to_float();

        fire_bullet();
    }
}

void Weapon::stop_fire(const std::string &subtype)
{
    if (weapon_subtype == subtype)
	{
        should_stop_fire = true;
    }
}

void Weapon::try_switch_to_weapon(const std::string &weapon)
{
    if (next_weapon_type.empty() && weapon_type != weapon)
	{
        next_weapon_type = weapon;
    }
}

void Weapon::fire_bullet()
{
	JsonValue subtype_description = player->world()->weapon_data["weapons"][weapon_type][weapon_subtype];

	if (subtype_description["type"].to_string() == "zoom")
		return;

	Vec3f offset(subtype_description["offset"]["x"].to_float(), subtype_description["offset"]["y"].to_float(), subtype_description["offset"]["z"].to_float());

	Vec3f bullet_pos = player->get_position() + player->eye_offset + player->get_orientation().rotate_vector(offset);

	player->world()->add(new Bullet(player->world(), subtype_description["bullet"].to_string(), bullet_pos, player->get_orientation()));
}

void Weapon::tick_ready(const GameTick &tick)
{
	if (!next_weapon_type.empty())
	{
		if (!weapon_type.empty())
		{
			if (!weapon_object.is_null())
				weapon_object.play_animation("hide", true);
			animation_timer = player->world()->weapon_data["weapons"][weapon_type]["hideTime"].to_float();
		}
		else
		{
			animation_timer = 0.0f;
		}

		weapon_state = hiding_old_weapon;
	}
}

void Weapon::tick_hiding_old_weapon(const GameTick &tick)
{
	if (animation_timer == 0.0f)
	{
		weapon_type = next_weapon_type;
		next_weapon_type.clear();

		weapon_state = showing_new_weapon;

		if (!weapon_type.empty())
			animation_timer = player->world()->weapon_data["weapons"][weapon_type]["showTime"].to_float();

		if (!player->world()->is_server)
		{
			JsonValue weapon_description = player->world()->weapon_data["weapons"][weapon_type];

			// Crosshair.setCrosshair(weaponDescription.crosshair);

			Vec3f weapon_offset(
				weapon_description["firstPersonView"]["position"]["x"].to_float(),
				weapon_description["firstPersonView"]["position"]["y"].to_float(),
				weapon_description["firstPersonView"]["position"]["z"].to_float());

			float dir = weapon_description["firstPersonView"]["orientation"]["dir"].to_float();
			float up = weapon_description["firstPersonView"]["orientation"]["up"].to_float();
			float tilt = weapon_description["firstPersonView"]["orientation"]["tilt"].to_float();

			auto model = SceneModel::create(player->world()->game()->scene, weapon_description["firstPersonView"]["mesh"].to_string());
			weapon_object = SceneObject(player->world()->game()->scene, model);
			weapon_object.set_scale(Vec3f(weapon_description["firstPersonView"]["scale"].to_float()));
			/*
			weapon_object.set_position(static_cast<ClientPlayerPawn*>(player)->camera.get_position());
			weapon_object.set_orientation(static_cast<ClientPlayerPawn*>(player)->camera.get_orientation());
			weapon_object.move(weapon_offset);
			weapon_object.rotate(dir, up, tilt);
			*/
			weapon_object.play_animation("show", true);
		}
	}
}

void Weapon::tick_showing_new_weapon(const GameTick &tick)
{
	if (animation_timer == 0.0f)
	{
		weapon_state = ready;

		if (!weapon_object.is_null())
			weapon_object.play_animation("default", true);
	}
}

void Weapon::tick_firing(const GameTick &tick)
{
	if (animation_timer == 0.0f)
	{
		JsonValue subweapon_description = player->world()->weapon_data["weapons"][weapon_type][weapon_subtype];

		std::string type = subweapon_description["type"].to_string();

		if (type == "semi" || should_stop_fire)
		{
			weapon_state = ready;
			should_stop_fire = false;
		}
		else if (type == "automatic")
		{
			animation_timer = subweapon_description["cooldown"].to_float();
			fire_bullet();
		}
	}
}

void Weapon::frame(float time_elapsed, float interpolated_time)
{
	if (!weapon_object.is_null())
	{
		JsonValue weapon_description = player->world()->weapon_data["weapons"][weapon_type];

		Vec3f weapon_offset(
			weapon_description["firstPersonView"]["position"]["x"].to_float(),
			weapon_description["firstPersonView"]["position"]["y"].to_float(),
			weapon_description["firstPersonView"]["position"]["z"].to_float());

		float dir = weapon_description["firstPersonView"]["orientation"]["dir"].to_float();
		float up = weapon_description["firstPersonView"]["orientation"]["up"].to_float();
		float tilt = weapon_description["firstPersonView"]["orientation"]["tilt"].to_float();

		weapon_object.set_position(static_cast<ClientPlayerPawn*>(player)->camera->position());
		weapon_object.set_orientation(static_cast<ClientPlayerPawn*>(player)->camera->orientation());
		weapon_object.move(weapon_offset);
		weapon_object.rotate(dir, up, tilt);

		weapon_object.update(time_elapsed);
	}
}
