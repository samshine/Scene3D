
#pragma once

#include "game_object.h"
#include "old_character_controller.h"
#include "weapon.h"

class PlayerPawn : public GameObject
{
public:
	PlayerPawn(GameWorld *world);
	~PlayerPawn();

	void tick(const GameTick &tick) override;

	void ground_moved(const clan::Vec3f &offset);

	clan::Vec3f get_position() { return controller->get_position(); }
	clan::Quaternionf get_orientation() { return clan::Quaternionf(up, dir, tilt, clan::angle_degrees, clan::order_YXZ); }

	const clan::Vec3f eye_offset = clan::Vec3f(0.0f, 4.0f, 0.0f);

	virtual void apply_damage(const GameTick &tick, float damage) { }

	Weapon *get_weapon() { return weapon.get(); }

protected:
	clan::Vec3f tick_controller_movement(float time_elapsed);

	std::unique_ptr<OldCharacterController> controller;

	float health = 100.0f;

	float up = 0.0f;
	float dir = 0.0f;
	float tilt = 0.0f;

	bool key_forward = false;
	bool key_back = false;
	bool key_left = false;
	bool key_right = false;
	bool key_jump = false;
	bool key_fire_primary = false;
	bool key_fire_secondary = false;
	int key_weapon = 0;

	const float move_speed = 440.0f;
	const float slide_speed = 320.0f;
	const float move_resistance = 0.75f;

	clan::Vec3f move_velocity;

	float animation_move_speed = 0.0f;

	std::unique_ptr<Weapon> weapon;
};
