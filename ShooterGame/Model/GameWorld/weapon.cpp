
#include "precomp.h"
#include "weapon.h"
#include "client_player_pawn.h"
#include "game_world.h"
#include "bullet.h"
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

        weapon_state = firing;
		animation_timer = weapon_subtype == "primary" ? weapons()[weapon_type].primary.cooldown : weapons()[weapon_type].secondary.cooldown;

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
	const auto &subtype = weapon_subtype == "primary" ? weapons()[weapon_type].primary : weapons()[weapon_type].secondary;

	if (subtype.type == WeaponFireType::zoom)
		return;

	Vec3f offset(0.2f, -0.2f, 0.0f);
	Vec3f bullet_pos = player->get_position() + player->eye_offset + player->get_orientation().rotate_vector(offset);

	player->world()->add(std::make_shared<Bullet>(player, subtype.bullet, bullet_pos, player->get_orientation()));
}

void Weapon::tick_ready(const GameTick &tick)
{
	if (!next_weapon_type.empty())
	{
		if (!weapon_type.empty())
		{
			if (weapon_object)
				weapon_object->play_animation("hide", true);
			animation_timer = weapons()[weapon_type].hide_time;
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
			animation_timer = weapons()[weapon_type].show_time;

		if (player->world()->client)
		{
			const auto &subtype = weapon_subtype == "primary" ? weapons()[weapon_type].primary : weapons()[weapon_type].secondary;

			const auto &weapon_description = weapons()[weapon_type];

			float dir = weapon_description.rotation.y;
			float up = weapon_description.rotation.x;
			float tilt = weapon_description.rotation.z;

			auto model = SceneModel::create(player->world()->client->scene, weapon_description.mesh);
			weapon_object = SceneObject::create(player->world()->client->scene, model);
			weapon_object->set_position(static_cast<ClientPlayerPawn*>(player)->camera->position());
			weapon_object->set_orientation(static_cast<ClientPlayerPawn*>(player)->camera->orientation());
			weapon_object->set_scale(Vec3f(weapon_description.scale));
			weapon_object->move(weapon_description.position);
			weapon_object->rotate(dir, up, tilt);
			weapon_object->play_animation("show", true);
		}
	}
}

void Weapon::tick_showing_new_weapon(const GameTick &tick)
{
	if (animation_timer == 0.0f)
	{
		weapon_state = ready;

		if (weapon_object)
			weapon_object->play_animation("default", true);
	}
}

void Weapon::tick_firing(const GameTick &tick)
{
	if (animation_timer == 0.0f)
	{
		const auto &subtype = weapon_subtype == "primary" ? weapons()[weapon_type].primary : weapons()[weapon_type].secondary;

		if (subtype.type == WeaponFireType::semi || should_stop_fire)
		{
			weapon_state = ready;
			should_stop_fire = false;
		}
		else if (subtype.type == WeaponFireType::automatic)
		{
			animation_timer = subtype.cooldown;
			fire_bullet();
		}
	}
}

void Weapon::frame(float time_elapsed, float interpolated_time)
{
	if (weapon_object)
	{
		const auto &weapon_description = weapons()[weapon_type];

		Vec3f weapon_offset = weapon_description.position;

		float dir = weapon_description.rotation.y;
		float up = weapon_description.rotation.x;
		float tilt = weapon_description.rotation.z;

		weapon_object->set_position(static_cast<ClientPlayerPawn*>(player)->camera->position());
		weapon_object->set_orientation(static_cast<ClientPlayerPawn*>(player)->camera->orientation());
		weapon_object->move(weapon_offset);
		weapon_object->rotate(dir, up, tilt);

		weapon_object->update(time_elapsed);
	}
}

std::map<std::string, WeaponDescription> &Weapon::weapons()
{
	static std::map<std::string, WeaponDescription> weapons =
	{
		{ "TractorBeam", { Vec3f(0.1f, -0.4f, 0.0f), Vec3f(2.0f, 170.0f, -3.0f), "Models/Tractor_beam/View.cmodel", 0.4f, { WeaponFireType::automatic, 0.5f, "Instagib" }, { WeaponFireType::automatic, 0.0001f, "Instagibeam" } } },
		{ "RocketLauncher", { Vec3f(0.2f, -0.27f, 0.05f), Vec3f(1.0f, 175.0f, -5.0f), "Models/Rocket_launcher/View.cmodel", 0.4f, { WeaponFireType::automatic, 1.0f, "Rocket" }, { WeaponFireType::automatic, 0.75f, "Grenade" } } },
		{ "DualGuns", { Vec3f(0.3f, -0.25f, 0.5f), Vec3f(1.0f, 180.0f, 0.0f), "Models/Dual_guns/View.cmodel", 0.003f, { WeaponFireType::automatic, 0.75f, "DualPrimary" }, { WeaponFireType::automatic, 1.25f, "DualSecondary" } } },
		{ "IceLauncher", { Vec3f(0.1f, -0.4f, 0.0f), Vec3f(2.0f, 170.0f, -3.0f), "Models/Ice_launcher/View.cmodel", 0.4f, { WeaponFireType::automatic, 0.2f, "IcePrimary" }, { WeaponFireType::automatic, 0.75f, "IceSecondary" } } },
		{ "SniperRifle", { Vec3f(0.17f, -0.4f, 0.4f), Vec3f(1.0f, -90.0f, 0.0f), "Models/Sniper_rifle/Model.cmodel", 0.16f, { WeaponFireType::automatic, 1.0f, "SniperPrimary" }, { WeaponFireType::zoom, 2.0f, "SniperSecondary" } } }
	};
	return weapons;
}
