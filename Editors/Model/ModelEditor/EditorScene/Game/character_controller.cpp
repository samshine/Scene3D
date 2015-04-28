
#include "precomp.h"
#include "character_controller.h"
#include <algorithm>

using namespace clan;

CharacterController::CharacterController(clan::Physics3DWorld collision_world) : collision_world(collision_world), sweep_test(collision_world)
{
}

void CharacterController::set_mass(float new_mass)
{
	mass = new_mass;
}

void CharacterController::set_height(float new_height)
{
	if (height != new_height)
	{
		height = new_height;
		shape_modified = true;
	}
}

void CharacterController::set_radius(float new_radius)
{
	if (radius != new_radius)
	{
		radius = new_radius;
		shape_modified = true;
	}
}

void CharacterController::set_step_height(float new_step_height)
{
	step_height = new_step_height;
}

void CharacterController::set_acceleration(float new_acceleration)
{
	acceleration = new_acceleration;
}

void CharacterController::set_run_speed(float new_run_speed)
{
	run_speed = new_run_speed;
}

void CharacterController::set_friction(float new_friction)
{
	friction = new_friction;
}

void CharacterController::set_air_resistance(float new_air_resistance)
{
	air_resistance = new_air_resistance;
}

void CharacterController::set_air_movement(float new_air_movement)
{
	air_movement = new_air_movement;
}

void CharacterController::set_bounce(float new_bounce)
{
	bounce = new_bounce;
}

void CharacterController::set_gravity(float new_gravity)
{
	gravity = new_gravity;
}

void CharacterController::warp(const clan::Vec3f &new_position, const EulerRotation &new_rotation, const clan::Vec3f &new_velocity)
{
	position = new_position;
	rotation = new_rotation;
	velocity = new_velocity;
}

void CharacterController::apply_impulse(const clan::Vec3f &force)
{
	velocity += force / mass;
	flying = true;
}

void CharacterController::thrust(const clan::Vec2f &dir)
{
	thrust_vec = dir;
}

void CharacterController::look(const EulerRotation &new_rotation)
{
	rotation = new_rotation;
}

void CharacterController::update(float tick_elapsed)
{
	update_shape();
	begin_step_up();
	apply_thrust(tick_elapsed);
	apply_velocity(tick_elapsed);
	apply_gravity(tick_elapsed);
	end_step_up();
	step_down();
}

void CharacterController::update_shape()
{
	if (shape_modified)
	{
		collision_shape = Physics3DShape::capsule(radius, height * 0.5f);
		shape_modified = false;
	}
}

bool CharacterController::step_move(clan::Vec3f move_vec)
{
	position.y += height * 0.5f + shape_margin;

	float vertical_sign = move_vec.y >= 0.0f ? 1.0f : -1.0f;

	bool hit_ceiling_or_floor = false;
	for (int iteration = 0; !hit_ceiling_or_floor && iteration < 3; iteration++)
	{
		Vec3f from_pos = position;
		Vec3f to_pos = position + move_vec;
		Quaternionf orientation;

		if (sweep_test.test_all_hits(collision_shape, from_pos, orientation, to_pos, orientation, allowed_ccd))
		{
			// Look for a floor/ceiling first as multiple faces might have contact at face edges
			float t = sweep_test.get_hit_fraction(0);
			for (int hit_index = 0; hit_index < sweep_test.get_hit_count(); hit_index++)
			{
				Vec3f normal = sweep_test.get_hit_normal(hit_index);

				float dot = Vec3f::dot(Vec3f(0.0f, vertical_sign, 0.0f), normal);
				if (dot < -acos_too_steep_slope || dot > acos_too_steep_slope)
				{
					position = sweep_test.get_hit_position(0);
					hit_ceiling_or_floor = true;
					break; // hit ceiling or floor
				}
			}

			// If we didn't hit a floor/ceiling we must have hit a steep wall, adjust movement vector and try again
			if (!hit_ceiling_or_floor)
			{
				float margin = allowed_ccd * 0.3f;
				Vec3f normal = sweep_test.get_hit_normal(0);
				move_vec = mix(move_vec, reflect(move_vec, normal, step_bounce), 1.0f - sweep_test.get_hit_fraction(0)) + normal * margin;
			}
		}
		else
		{
			position = to_pos;
			break;
		}
	}

	position.y -= height * 0.5f + shape_margin;

	return hit_ceiling_or_floor;
}

void CharacterController::begin_step_up()
{
	if (velocity.y > 0.0f)
		flying = true;

	step_move(Vec3f(0.0f, step_height, 0.0f));
}

void CharacterController::end_step_up()
{
	if (step_move(Vec3f(0.0f, -step_height, 0.0f)))
	{
		velocity.y = 0.0f;
		flying = false;
	}
}

void CharacterController::step_down()
{
	if (!flying)
	{
		if (step_move(Vec3f(0.0f, -step_height, 0.0f)))
		{
			velocity.y = 0.0f;
			flying = false;
		}
		else
		{
			flying = true;
		}
	}
}

void CharacterController::apply_gravity(float tick_elapsed)
{
	velocity.y -= gravity * tick_elapsed;
}

void CharacterController::apply_velocity(float tick_elapsed)
{
	position.y += height * 0.5f + shape_margin;

	float t = 0.0f;
	for (int iteration = 0; iteration < 5 && t < 1.0f; iteration++)
	{
		float velocity_length = velocity.length();
		if (velocity_length < 0.001f)
		{
			velocity = Vec3f();
			break;
		}

		Vec3f from_pos = position;
		Vec3f to_pos = position + velocity * (1.0f - t) * tick_elapsed;
		Quaternionf orientation;
		bool found = false;
		sweep_test.test_all_hits(collision_shape, from_pos, orientation, to_pos, orientation, allowed_ccd);
		for (int hit = 0; hit < sweep_test.get_hit_count(); hit++)
		{
			//if (Vec3f::dot(velocity, sweep_test.get_hit_normal(hit)) > 0)
			//	continue;

			position = sweep_test.get_hit_position(hit);
			velocity = reflect(velocity, sweep_test.get_hit_normal(hit), bounce);

			t = t + sweep_test.get_hit_fraction(hit) * (1.0f - t);

			found = true;
			break;
		}

		if (!found)
		{
			position = to_pos;
			t = 1.0f;
		}
	}

	position.y -= height * 0.5f + shape_margin;
}

void CharacterController::apply_thrust(float tick_elapsed)
{
	velocity -= velocity * air_resistance * tick_elapsed;

	Vec2f delta = thrust_vec * run_speed - ground_vec;
	float delta_length = delta.length();
	if (delta_length > 0.0f)
	{
		ground_vec += delta * (std::min(delta_length, acceleration * tick_elapsed) / delta_length);
	}

	Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

	Vec3f wanted_velocity = move_direction.rotate_vector(Vec3f(ground_vec.x, 0.0f, ground_vec.y));
	float wanted_acceleration = acceleration;
	if (flying)
	{
		wanted_acceleration *= air_movement;

		if (wanted_velocity.length() < 0.001f)
			wanted_velocity = velocity;
	}

	Vec3f change_vector = wanted_velocity - velocity;
	float distance = change_vector.length();
	if (distance > 0.0f)
	{
		change_vector *= 1.0f / distance;
		velocity.x += change_vector.x * std::min(wanted_acceleration * tick_elapsed, distance);
		velocity.z += change_vector.z * std::min(wanted_acceleration * tick_elapsed, distance);
	}
}

Vec3f CharacterController::reflect(const Vec3f &ray, const Vec3f &normal, float amount)
{
	return ray - normal * ((1.0f + amount) * ray.dot(normal));
}
