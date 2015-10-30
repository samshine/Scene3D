
#include "precomp.h"
#include "robot_player_pawn.h"
#include "game_world.h"

using namespace uicore;

RobotPlayerPawn::RobotPlayerPawn(GameWorld *world, const std::string &owner, std::shared_ptr<SpawnPoint> spawn) : ServerPlayerPawn(world, owner, spawn)
{
}

void RobotPlayerPawn::tick(const GameTick &tick)
{
	ServerPlayerPawn::tick(tick);

	cur_movement.key_forward.next_pressed = false;
	cur_movement.key_back.next_pressed = false;
	cur_movement.key_left.next_pressed = false;
	cur_movement.key_right.next_pressed = false;
	cur_movement.key_jump.next_pressed = false;
	cur_movement.key_fire_primary.next_pressed = false;
	cur_movement.key_fire_secondary.next_pressed = false;
	cur_movement.key_weapon = 0;

	move_cooldown = std::max(move_cooldown - tick.time_elapsed, 0.0f);
	weapon_change_cooldown = std::max(weapon_change_cooldown - tick.time_elapsed, 0.0f);

	switch (mode)
	{
	case RobotPlayerMode::idle: tick_idle(tick); break;
	case RobotPlayerMode::follow: tick_follow(tick); break;
	}
}

void RobotPlayerPawn::tick_idle(const GameTick &tick)
{
}

void RobotPlayerPawn::tick_follow(const GameTick &tick)
{
	std::shared_ptr<ServerPlayerPawn> target = nullptr;

	for (auto it : world()->server_player_pawns)
	{
		auto other_pawn = it.second;
		if (other_pawn.get() != this)
			target = other_pawn;
	}

	if (target)
	{
		auto eye_pos = get_position() + eye_offset;
		auto target_eye_pos = target->get_position() + eye_offset;

		bool line_of_sight = !world()->collision->ray_test_any(eye_pos, target_eye_pos);

		if (line_of_sight)
			last_seen_target_pos = target_eye_pos;

		auto target_pos = Vec2f(last_seen_target_pos.x, last_seen_target_pos.z);
		auto robot_pos = Vec2f(eye_pos.x, eye_pos.z);
		auto target_dir = Vec2f::normalize(target_pos - robot_pos);

		float wanted_angle = std::acos(target_dir.y) * 180.0f / PI;
		if (target_dir.x < 0.0f)
			wanted_angle = 360.0f - wanted_angle;

		float delta = wanted_angle - cur_movement.dir;
		if (delta < -180.0f)
			delta += 360.0f;
		else if (delta > 180.0f)
			delta -= 360.0f;

		float turn_speed = 120.0f * tick.time_elapsed;

		if (std::abs(delta) < turn_speed)
			cur_movement.dir = wanted_angle;
		else if (delta >= 0.0f)
			cur_movement.dir += turn_speed;
		else
			cur_movement.dir -= turn_speed;

		cur_movement.up = -std::asin(Vec3f::normalize(last_seen_target_pos - eye_pos).y) * 180.0f / PI;

		float shoot_fov = 10.0f;
		if (std::abs(cur_movement.dir - wanted_angle) < shoot_fov && line_of_sight)
		{
			cur_movement.key_fire_primary.next_pressed = true;
			if (move_cooldown == 0.0f)
				cur_movement.key_forward.next_pressed = true;
		}
	}

	if (weapon_change_cooldown == 0.0f)
	{
		weapon_change_cooldown = 15.0f;

		float random = rand() / (float)RAND_MAX;
		int weapon_index = (int)std::round(5 * random);
		cur_movement.key_weapon = weapon_index;
	}
}
