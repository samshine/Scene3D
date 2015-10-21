
#include "precomp.h"
#include "player_pawn.h"
#include "game_world.h"
#include "game_tick.h"
#include "game_object_collision.h"
#include <algorithm>

using namespace uicore;

PlayerPawn::PlayerPawn(GameWorld *world) : GameObject(world), character_controller(world->collision)
{
	//controller.reset(new OldCharacterController(world->collision, 0.5f, 1.8f*0.5f, 0.25f, 58.0f));
	//controller->get_object().set_data(std::make_shared<GameObjectCollision>(this));

	weapon.reset(new Weapon(this));
}

PlayerPawn::~PlayerPawn()
{
}

void PlayerPawn::tick(const GameTick &tick)
{
	bool was_moving = animation_move_speed > 0.0f;

	update_character_controller(tick.time_elapsed);

	switch (cur_movement.key_weapon)
	{
	case 1: weapon->try_switch_to_weapon("IceLauncher"); break;
	case 2: weapon->try_switch_to_weapon("RocketLauncher"); break;
	case 3: weapon->try_switch_to_weapon("DualGuns"); break;
	case 4: weapon->try_switch_to_weapon("SniperRifle"); break;
	case 5: weapon->try_switch_to_weapon("TractorBeam"); break;
	}

	if (cur_movement.key_fire_primary.pressed)
		weapon->start_fire("primary");
	else
		weapon->stop_fire("primary");

	if (cur_movement.key_fire_secondary.pressed)
		weapon->start_fire("secondary");
	else
		weapon->stop_fire("secondary");

	weapon->tick(tick);
}

void PlayerPawn::ground_moved(const Vec3f &offset)
{
	//controller->set_position(controller->get_position() + offset);
}

void PlayerPawn::update_character_controller(float time_elapsed)
{
	cur_movement.key_forward.update(time_elapsed);
	cur_movement.key_back.update(time_elapsed);
	cur_movement.key_left.update(time_elapsed);
	cur_movement.key_right.update(time_elapsed);
	cur_movement.key_jump.update(time_elapsed);
	cur_movement.key_fire_primary.update(time_elapsed);
	cur_movement.key_fire_secondary.update(time_elapsed);

	EulerRotation rotation = character_controller.get_rotation();

	Vec2f thrust;
	if (cur_movement.key_left.pressed)
		thrust.x -= 1.0f;
	if (cur_movement.key_right.pressed)
		thrust.x += 1.0f;
	if (cur_movement.key_forward.pressed)
		thrust.y += 1.0f;
	if (cur_movement.key_back.pressed)
		thrust.y -= 1.0f;
	thrust.normalize();

	anim = "default";
	if (thrust.x < -0.1f)
		anim = "strafe-left";
	else if (thrust.x > 0.1f)
		anim = "strafe-right";
	else if (thrust.y > 0.1f)
		anim = "forward";
	else if (thrust.y < -0.1f)
		anim = "backward";

	cur_movement.dodge_cooldown = std::max(cur_movement.dodge_cooldown - time_elapsed, 0.0f);

	if (cur_movement.key_jump.clicked && !character_controller.is_flying())
	{
		character_controller.apply_impulse(Vec3f(0.0f, 500.0f, 0.0f));
		anim = "jump";
	}
	else if (cur_movement.dodge_cooldown == 0.0f && !character_controller.is_flying())
	{
		if (cur_movement.key_forward.double_clicked)
		{
			Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(0.0f, 25.0f, 94.0f) * 8.0f));
			cur_movement.dodge_cooldown = 0.75f;
			anim = "dodge-forward";
		}
		else if (cur_movement.key_back.double_clicked)
		{
			Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(0.0f, 25.0f, -94.0f) * 8.0f));
			cur_movement.dodge_cooldown = 0.75f;
			anim = "dodge-backward";
		}
		else if (cur_movement.key_left.double_clicked)
		{
			Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(-94.0f, 25.0f, 0.0f) * 8.0f));
			cur_movement.dodge_cooldown = 0.75f;
			anim = "dodge-left";
		}
		else if (cur_movement.key_right.double_clicked)
		{
			Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(94.0f, 25.0f, 0.0f) * 8.0f));
			cur_movement.dodge_cooldown = 0.75f;
			anim = "dodge-right";
		}
	}
	else if (character_controller.is_flying())
	{
		anim = "jump";
	}

	character_controller.look(EulerRotation(cur_movement.dir, cur_movement.up));
	character_controller.thrust(thrust);

	Vec3f old_pos = character_controller.get_position();

	character_controller.update(time_elapsed);

	Vec3f new_pos = character_controller.get_position();
	animation_move_speed = (Vec2f(new_pos.x, new_pos.z) - Vec2f(old_pos.x, old_pos.z)).length();
}
