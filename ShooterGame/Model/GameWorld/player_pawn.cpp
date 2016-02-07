
#include "precomp.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

PlayerPawn::PlayerPawn() : character_controller(game_world()->kinematic_collision())
{
	auto collision_obj = Physics3DObject::collision_body(game_world()->kinematic_collision(), Physics3DShape::capsule(character_controller.get_radius(), character_controller.get_height() * 0.5f));
	//collision_obj->set_kinematic_object();
	collision_obj->set_character_object();
	set_collision_object(collision_obj);

	weapon.reset(new Weapon(this));
}

PlayerPawn::~PlayerPawn()
{
}

void PlayerPawn::tick()
{
	bool was_moving = animation_move_speed > 0.0f;

	update_input();
	update_character_controller(last_movement);

	switch (key_weapon)
	{
	case 1: weapon->try_switch_to_weapon("IceLauncher"); break;
	case 2: weapon->try_switch_to_weapon("RocketLauncher"); break;
	case 3: weapon->try_switch_to_weapon("DualGuns"); break;
	case 4: weapon->try_switch_to_weapon("SniperRifle"); break;
	case 5: weapon->try_switch_to_weapon("TractorBeam"); break;
	}

	if (key_fire_primary.pressed)
		weapon->start_fire("primary");
	else
		weapon->stop_fire("primary");

	if (key_fire_secondary.pressed)
		weapon->start_fire("secondary");
	else
		weapon->stop_fire("secondary");

	weapon->tick(time_elapsed());
}

void PlayerPawn::ground_moved(const Vec3f &offset)
{
	//controller->set_position(controller->get_position() + offset);
}

void PlayerPawn::update_input()
{
	PlayerPawnMovement movement;

	movement.tick_time = send_tick_time();

	key_forward.update(time_elapsed());
	key_back.update(time_elapsed());
	key_left.update(time_elapsed());
	key_right.update(time_elapsed());
	key_jump.update(time_elapsed());
	key_fire_primary.update(time_elapsed());
	key_fire_secondary.update(time_elapsed());

	if (key_left.pressed)
		movement.thrust.x -= 1.0f;
	if (key_right.pressed)
		movement.thrust.x += 1.0f;
	if (key_forward.pressed)
		movement.thrust.y += 1.0f;
	if (key_back.pressed)
		movement.thrust.y -= 1.0f;
	movement.thrust.normalize();

	dodge_cooldown = std::max(dodge_cooldown - time_elapsed(), 0.0f);

	if (key_jump.clicked)
	{
		movement.action = PlayerPawnAction::jump;
	}
	else if (dodge_cooldown == 0.0f)
	{
		if (key_forward.double_clicked)
		{
			movement.action = PlayerPawnAction::dodge_forward;
			dodge_cooldown = 0.75f;
		}
		else if (key_back.double_clicked)
		{
			movement.action = PlayerPawnAction::dodge_backward;
			dodge_cooldown = 0.75f;
		}
		else if (key_left.double_clicked)
		{
			movement.action = PlayerPawnAction::dodge_left;
			dodge_cooldown = 0.75f;
		}
		else if (key_right.double_clicked)
		{
			movement.action = PlayerPawnAction::dodge_right;
			dodge_cooldown = 0.75f;
		}
	}

	movement.dir = dir;
	movement.up = up;

	last_movement = movement;
}

void PlayerPawn::update_character_controller(const PlayerPawnMovement &movement)
{
	anim = "default";
	if (movement.thrust.x < -0.1f)
		anim = "strafe-left";
	else if (movement.thrust.x > 0.1f)
		anim = "strafe-right";
	else if (movement.thrust.y > 0.1f)
		anim = "forward";
	else if (movement.thrust.y < -0.1f)
		anim = "backward";

	if (!character_controller.is_flying())
	{
		Quaternionf move_direction(0.0f, movement.dir, 0.0f, angle_degrees, order_YXZ);

		switch (movement.action)
		{
		case PlayerPawnAction::jump:
			character_controller.apply_impulse(Vec3f(0.0f, 500.0f, 0.0f));
			anim = "jump";
			break;

		case PlayerPawnAction::dodge_forward:
			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(0.0f, 25.0f, 94.0f) * 8.0f));
			anim = "dodge-forward";
			break;

		case PlayerPawnAction::dodge_backward:
			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(0.0f, 25.0f, -94.0f) * 8.0f));
			anim = "dodge-backward";
			break;

		case PlayerPawnAction::dodge_left:
			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(-94.0f, 25.0f, 0.0f) * 8.0f));
			anim = "dodge-left";
			break;

		case PlayerPawnAction::dodge_right:
			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(94.0f, 25.0f, 0.0f) * 8.0f));
			anim = "dodge-right";
			break;
		}
	}
	else if (character_controller.is_flying())
	{
		anim = "jump";
	}

	character_controller.look(EulerRotation(movement.dir, movement.up));
	character_controller.thrust(movement.thrust);

	Vec3f old_pos = character_controller.get_position();

	character_controller.update(time_elapsed());

	Vec3f new_pos = character_controller.get_position();
	animation_move_speed = (Vec2f(new_pos.x, new_pos.z) - Vec2f(old_pos.x, old_pos.z)).length();

	set_collision_position(new_pos + Vec3f(0.0f, character_controller.get_height() * 0.5f, 0.0f));
}
