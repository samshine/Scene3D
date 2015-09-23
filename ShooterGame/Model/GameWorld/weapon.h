
#pragma once

#include "game_tick.h"

class PlayerPawn;

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

	uicore::SceneObject weapon_object;
};
