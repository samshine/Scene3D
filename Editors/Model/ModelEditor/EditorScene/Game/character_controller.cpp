
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
	apply_gravity(tick_elapsed);
	apply_thrust(tick_elapsed);
	apply_velocity(tick_elapsed);
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

void CharacterController::begin_step_up()
{
	if (velocity.y > 0.0f)
		flying = true;

	position.y += height * 0.5f;

	Vec3f move_vec(0.0f, step_height, 0.0f);

	float t = 0.0f;
	for (int iteration = 0; iteration < 3 && t < 1.0f; iteration++)
	{
		Vec3f from_pos = position;
		Vec3f to_pos = position + move_vec * (1.0f - t);
		Quaternionf orientation;

		if (sweep_test.test_first_hit(collision_shape, from_pos, orientation, to_pos, orientation, allowed_ccd))
		{
			position = sweep_test.get_hit_position(0);
			move_vec = reflect(move_vec, sweep_test.get_hit_normal(0), step_bounce);
			t = t + sweep_test.get_hit_fraction(0) * (1.0f - t);

			float dot = Vec3f::dot(Vec3f(0.0f, 1.0f, 0.0f), sweep_test.get_hit_normal(0));
			if (dot < -acos_too_steep_slope || dot > acos_too_steep_slope)
			{
				break; // hit ceiling or floor
			}
		}
		else
		{
			position = to_pos;
			t = 1.0f;
		}
	}

	position.y -= height * 0.5f;
}

void CharacterController::end_step_up()
{
	position.y += height * 0.5f;

	Vec3f move_vec(0.0f, -step_height, 0.0f);

	float t = 0.0f;
	for (int iteration = 0; iteration < 3 && t < 1.0f; iteration++)
	{
		Vec3f from_pos = position;
		Vec3f to_pos = position + move_vec * (1.0f - t);
		Quaternionf orientation;

		if (sweep_test.test_first_hit(collision_shape, from_pos, orientation, to_pos, orientation, allowed_ccd))
		{
			position = sweep_test.get_hit_position(0);
			move_vec = reflect(move_vec, sweep_test.get_hit_normal(0), step_bounce);
			t = t + sweep_test.get_hit_fraction(0) * (1.0f - t);

			float dot = Vec3f::dot(Vec3f(0.0f, -1.0f, 0.0f), sweep_test.get_hit_normal(0));
			if (dot < -acos_too_steep_slope || dot > acos_too_steep_slope)
			{
				velocity.y = 0.0f;
				flying = false;
				break; // hit ceiling or floor
			}
		}
		else
		{
			position = to_pos;
			t = 1.0f;
		}
	}

	position.y -= height * 0.5f;
}

void CharacterController::step_down()
{
	if (!flying)
	{
		Vec3f new_position = position;
		new_position.y += height * 0.5f;

		Vec3f move_vec(0.0f, -step_height, 0.0f);

		float t = 0.0f;
		for (int iteration = 0; iteration < 3 && t < 1.0f; iteration++)
		{
			Vec3f from_pos = new_position;
			Vec3f to_pos = new_position + move_vec * (1.0f - t);
			Quaternionf orientation;

			if (sweep_test.test_first_hit(collision_shape, from_pos, orientation, to_pos, orientation, allowed_ccd))
			{
				new_position = sweep_test.get_hit_position(0);
				move_vec = reflect(move_vec, sweep_test.get_hit_normal(0), step_bounce);
				t = t + sweep_test.get_hit_fraction(0) * (1.0f - t);

				float dot = Vec3f::dot(Vec3f(0.0f, -1.0f, 0.0f), sweep_test.get_hit_normal(0));
				if (dot < -acos_too_steep_slope || dot > acos_too_steep_slope)
				{
					break; // hit ceiling or floor
				}
			}
			else
			{
				new_position = to_pos;
				t = 1.0f;
			}
		}

		new_position.y -= height * 0.5f;

		if (t < 1.0f)
		{
			position = new_position;
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
	position.y += height * 0.5f;

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

	position.y -= height * 0.5f;
}

void CharacterController::apply_thrust(float tick_elapsed)
{
	Vec2f delta = thrust_vec * run_speed - ground_vec;
	float delta_length = delta.length();
	if (delta_length > 0.0f)
	{
		ground_vec += delta * (std::min(delta_length, acceleration * tick_elapsed) / delta_length);
	}

	Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

	if (flying)
	{
		velocity += move_direction.rotate_vector(Vec3f(ground_vec.x, 0.0f, ground_vec.y) * air_movement * tick_elapsed);
	}
	else
	{
		velocity = move_direction.rotate_vector(Vec3f(ground_vec.x, 0.0f, ground_vec.y));
	}
}

Vec3f CharacterController::reflect(const Vec3f &ray, const Vec3f &normal, float amount)
{
	return ray - normal * ((1.0f + amount) * ray.dot(normal));
}
