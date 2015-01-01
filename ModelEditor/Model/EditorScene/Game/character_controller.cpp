
#include "precomp.h"
#include "character_controller.h"

using namespace clan;

CharacterController::CharacterController(clan::Physics3DWorld collision_world) : collision_world(collision_world), sweep_test(collision_world)
{
}

void CharacterController::set_mass(float new_mass)
{
	mass = new_mass;
}

void CharacterController::set_size(float new_height, float new_radius)
{
	if (height != new_height || radius != new_radius)
	{
		height = new_height;
		radius = new_radius;
		shape_modified = true;
	}
}

void CharacterController::set_max_speed(float new_max_speed)
{
	max_speed = new_max_speed;
}

void CharacterController::set_air_resistance(float new_air_resistance)
{
	air_resistance = new_air_resistance;
}

void CharacterController::set_air_movement(float new_air_movement)
{
	air_movement = new_air_movement;
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
	apply_gravity(tick_elapsed);
	apply_air_resistance(tick_elapsed);
	apply_thrust(tick_elapsed);
	apply_velocity(tick_elapsed);
}

void CharacterController::update_shape()
{
	if (shape_modified)
	{
		collision_shape = Physics3DShape::capsule(radius, height * 0.5f);
		shape_modified = false;
	}
}

void CharacterController::apply_gravity(float tick_elapsed)
{
	velocity.y -= gravity * tick_elapsed;
}

void CharacterController::apply_air_resistance(float tick_elapsed)
{
	velocity.x -= velocity.x * air_resistance * tick_elapsed;
	velocity.z -= velocity.z * air_resistance * tick_elapsed;
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
			velocity = reflect(velocity, sweep_test.get_hit_normal(hit), 0.25f);

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
	clan::Quaternionf thrust_orientation(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);
	velocity += thrust_orientation.rotate_vector(Vec3f(thrust_vec.x, 0.0f, thrust_vec.y) * tick_elapsed);
}

Vec3f CharacterController::reflect(const Vec3f &ray, const Vec3f &normal, float amount)
{
	return ray - normal * ((1.0f + amount) * ray.dot(normal));
}
