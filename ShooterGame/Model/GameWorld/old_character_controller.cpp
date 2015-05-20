
#include "precomp.h"
#include "old_character_controller.h"
#include "game_object.h"
#include "game_object_collision.h"
#include "player_pawn.h"
#include <algorithm>

using namespace clan;

OldCharacterController::OldCharacterController(Physics3DWorld &world, float radius, float height, float step_height, float gravity)
: height_offset(-height), step_height(step_height), gravity(gravity), flying(true), contact_test(world), sweep_test(world), allowed_ccd_penetration(0.01f), margin(0.01f)
{
	shape = Physics3DShape::capsule(radius, height);
	shape2 = Physics3DShape::capsule(radius + 0.02f, height);

	object = Physics3DObject::collision_body(world, shape, position);
	object.set_character_object(true);
}

Vec3f OldCharacterController::get_position() const
{
	return position + Vec3f(0.0f, height_offset, 0.0f);
}

void OldCharacterController::set_position(const clan::Vec3f &new_position)
{
	position = new_position - Vec3f(0.0f, height_offset, 0.0f);
	object.set_position(position);
}

void OldCharacterController::move(const Vec3f &velocity, float time_elapsed)
{
	recover_from_penetration();

	if (flying)
	{
		// Apply gravity (disable for water / space)
		fly_velocity -= Vec3f(0.0f, gravity, 0.0f) * time_elapsed;

		// Apply air movement
		fly_velocity += velocity * (time_elapsed * air_speed);

		if (move_vertical(position + fly_velocity * time_elapsed))
		{
			if (fly_velocity.y > 0.0f)
			{
				fly_velocity.y = 0.0f;
			}
			else
			{
				flying = false;
				fly_velocity = Vec3f();
			}
		}
	}
	else if (velocity != Vec3f())
	{
		// Move position upwards to help walk on stairs:
		move_vertical(position + Vec3f(0.0f, step_height, 0.0f));

		// Move position forwards:
		move_forward(position + velocity * time_elapsed);

		// Move position downwards to help walk on stairs:
		if (!move_vertical(position - Vec3f(0.0f, 2.0f * step_height, 0.0f)))
		{
			flying = true;
			fly_velocity = velocity;
		}
	}

	object.set_position(position);
}

void OldCharacterController::recover_from_penetration()
{
	contact_test.test(object);

	float max_penetration = 0.0f;
	Vec3f touching_normal;
	for (int i = 0; i < contact_test.get_hit_count(); i++)
	{
		if (contact_test.get_hit_distance(i) < max_penetration)
		{
			max_penetration = contact_test.get_hit_distance(i);
			touching_normal = contact_test.get_hit_normal(i);
		}
	}

	/*if (max_penetration != 0.0f) {
		Console::write_line("Max penetration: %1, touching_normal: (%2,%3,%4)", max_penetration, touching_normal.x, touching_normal.y, touching_normal.z);
	}*/

	position += touching_normal * (max_penetration - 0.02f);
}

void OldCharacterController::jump(const clan::Vec3f &jump_velocity)
{
	if (!flying)
	{
		flying = true;
		fly_velocity = jump_velocity;
	}
}

void OldCharacterController::move_forward(clan::Vec3f target)
{
	const int max_iterations = 4;

	float move_length = (target - position).length();
	Vec3f direction = Vec3f::normalize(target - position);

	if (move_length < margin)
		return;

	for (int iterations = 0; iterations < max_iterations; iterations++)
	{
		Vec3f move_to = position + direction * move_length;

		sweep_test.test_all_hits(shape, position + direction * margin, Quaternionf(), move_to, Quaternionf(), allowed_ccd_penetration);

		int hit_index = -1;
		float hit_fraction = 1.0f;
		for (int i = 0; i < sweep_test.get_hit_count(); i++)
		{
			std::shared_ptr<GameObjectCollision> obj_collision = sweep_test.get_hit_object(i).get_data<GameObjectCollision>();
			GameObject *hit_game_object = obj_collision ? obj_collision->obj : 0;
			PlayerPawn *hit_player = dynamic_cast<PlayerPawn*>(hit_game_object);
			if (!hit_player)
			{
				if (sweep_test.get_hit_fraction(i) < hit_fraction)
				{
					hit_fraction = sweep_test.get_hit_fraction(i);
					hit_index = i;
				}
			}
		}

		if (hit_fraction != 1.0f)
		{
			if (iterations + 1 == max_iterations)
			{
				float length = std::max(sweep_test.get_hit_fraction(hit_index) * move_length, 0.0f);
				if (move_length <= margin)
					return;

				position = sweep_test.get_hit_position(hit_index); //position + direction * length;
				break;
			}
			else
			{
				Vec3f hit_normal = sweep_test.get_hit_normal(hit_index);
				Vec3f reflect_dir = reflect(direction, hit_normal);
				Vec3f perpendicular_dir = perpendicular_component(reflect_dir, hit_normal);

				direction = perpendicular_dir;
			}
		}
		else
		{
			position = move_to;
			break;
		}
	}
}

bool OldCharacterController::move_vertical(clan::Vec3f target)
{
	const int max_iterations = 2;

	float move_length = (target - position).length();
	Vec3f direction = Vec3f::normalize(target - position);

	if (move_length < margin)
		return false;
		
	for (int iterations = 0; iterations < max_iterations; iterations++)
	{
		Vec3f move_to = position + direction * move_length;

		sweep_test.test_all_hits(shape2, position + direction * margin, Quaternionf(), move_to, Quaternionf(), allowed_ccd_penetration);

		int hit_index = -1;
		float hit_fraction = 1.0f;
		for (int i = 0; i < sweep_test.get_hit_count(); i++)
		{
			std::shared_ptr<GameObjectCollision> obj_collision = sweep_test.get_hit_object(i).get_data<GameObjectCollision>();
			GameObject *hit_game_object = obj_collision ? obj_collision->obj : 0;
			PlayerPawn *hit_player = dynamic_cast<PlayerPawn*>(hit_game_object);
			if (!hit_player)
			{
				if (sweep_test.get_hit_fraction(i) < hit_fraction)
				{
					hit_fraction = sweep_test.get_hit_fraction(i);
					hit_index = i;
				}
			}
		}

		if (hit_fraction != 1.0f)
		{
			Vec3f hit_normal = sweep_test.get_hit_normal(hit_index);
			bool too_steep = false;

			if (direction.y > 0.0f) // moving up
			{
				too_steep = false; // No sliding when moving upwards
			}
			else // moving down
			{
				//too_steep = hit_normal.y < 0.75f;
			}

			if (!too_steep || iterations + 1 == max_iterations)
			{
				float length = std::max(sweep_test.get_hit_fraction(hit_index) * move_length, 0.0f);
				if (move_length <= margin)
					return false;

				position = sweep_test.get_hit_position(hit_index); //position + direction * length;
				return true;
			}
			else
			{
				Vec3f reflect_dir = reflect(direction, hit_normal);
				Vec3f perpendicular_dir = perpendicular_component(reflect_dir, hit_normal);

				direction = perpendicular_dir;
			}
		}
		else
		{
			position = target;
			break;
		}
	}

	return false;
}

Vec3f OldCharacterController::reflect(const Vec3f &direction, const Vec3f &normal)
{
	return direction - normal * (2.0f * direction.dot(normal));
}

Vec3f OldCharacterController::parallel_component(const Vec3f &direction, const Vec3f &normal)
{
	float magnitude = direction.dot(normal);
	return normal * magnitude;
}

Vec3f OldCharacterController::perpendicular_component(const Vec3f &direction, const Vec3f &normal)
{
	return direction - parallel_component(direction, normal);
}
