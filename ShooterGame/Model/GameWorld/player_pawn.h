
#pragma once

#include "game_object.h"
#include "character_controller.h"
#include "weapon.h"

class PlayerPawnButtonState
{
public:
	bool pressed = false;
	bool clicked = false;
	bool double_clicked = false;
	float double_click_timer = 0.0f;

	bool next_pressed = false;

	void update(float time_elapsed)
	{
		clicked = !pressed && next_pressed;
		pressed = next_pressed;
		double_clicked = false;

		if (clicked && double_click_timer > 0.0f)
		{
			double_clicked = true;
			double_click_timer = 0.0f;
		}
		else if (clicked)
		{
			double_click_timer = 0.3f;
		}

		double_click_timer = std::max(double_click_timer - time_elapsed, 0.0f);
	}
};

class PlayerPawnMovement
{
public:
	int tick_time = 0;

	// Input state before move
	PlayerPawnButtonState key_forward;
	PlayerPawnButtonState key_back;
	PlayerPawnButtonState key_left;
	PlayerPawnButtonState key_right;
	PlayerPawnButtonState key_jump;
	PlayerPawnButtonState key_fire_primary;
	PlayerPawnButtonState key_fire_secondary;
	int key_weapon = 0;
	float dir = 0.0f;
	float up = 0.0f;

	float dodge_cooldown = 0.0f;
};

class PlayerPawn : public GameObject
{
public:
	PlayerPawn(GameWorld *world);
	~PlayerPawn();

	void tick(const GameTick &tick) override;

	void ground_moved(const uicore::Vec3f &offset);

	uicore::Vec3f get_position() { return character_controller.get_position(); }
	uicore::Quaternionf get_orientation() { return uicore::Quaternionf(cur_movement.up, cur_movement.dir, 0.0f, uicore::angle_degrees, uicore::order_YXZ); }

	const uicore::Vec3f eye_offset = uicore::Vec3f(0.0f, 1.8f, 0.0f);

	virtual void apply_damage(const GameTick &tick, float damage) { }

	Weapon *get_weapon() { return weapon.get(); }

protected:
	void update_character_controller(float time_elapsed);

	PlayerPawnMovement cur_movement;
	CharacterController character_controller;

	float health = 100.0f;

	std::string anim = "default";
	float animation_move_speed = 0.0f;

	std::unique_ptr<Weapon> weapon;
};
