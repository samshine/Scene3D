
#pragma once

#include "game_tick.h"

class PlayerPawn;

enum class WeaponFireType
{
	automatic,
	semi,
	zoom
};

class WeaponFireMode
{
public:
	WeaponFireMode() { }
	WeaponFireMode(WeaponFireType type, float cooldown, std::string bullet) : type(type), cooldown(cooldown), bullet(bullet) { }

	WeaponFireType type;
	float cooldown = 0.5f;
	std::string bullet;
};

class WeaponDescription
{
public:
	WeaponDescription() { }
	WeaponDescription(uicore::Vec3f position, uicore::Vec3f rotation, std::string mesh, float scale, WeaponFireMode primary, WeaponFireMode secondary) : position(position), rotation(rotation), mesh(mesh), scale(scale), primary(primary), secondary(secondary) { }

	uicore::Vec3f position;
	uicore::Vec3f rotation;
	std::string mesh;
	float scale = 1.0f;
	float show_time = 0.5f;
	float hide_time = 0.5f;
	WeaponFireMode primary;
	WeaponFireMode secondary;
};

class Weapon
{
public:
	Weapon(PlayerPawn *player);
	~Weapon();

	void tick(const GameTick &tick);
	void frame(float time_elapsed, float interpolated_time);

	void start_fire(const std::string &subtype);
	void stop_fire(const std::string &subtype);
	void try_switch_to_weapon(const std::string &weapon);

	const std::string &get_weapon_type() const { return weapon_type; }

	static std::map<std::string, WeaponDescription> &weapons();

private:
	void tick_ready(const GameTick &tick);
	void tick_hiding_old_weapon(const GameTick &tick);
	void tick_showing_new_weapon(const GameTick &tick);
	void tick_firing(const GameTick &tick);

	void fire_bullet();

	PlayerPawn *player;

	enum WeaponState
	{
		ready,
		hiding_old_weapon,
		showing_new_weapon,
		firing
	};

	bool primary_fire_was_down = false;
	bool secondary_fire_was_down = false;

	std::string weapon_type;
	std::string next_weapon_type;
	float animation_timer = 0.0f;

	WeaponState weapon_state = ready;
	std::string weapon_subtype = "primary";
	bool should_stop_fire = false;

	SceneObjectPtr weapon_object;
};
