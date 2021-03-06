
#include "precomp.h"
#include "character_controller.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

CharacterController::CharacterController(const Physics3DWorldPtr &collision_world) : collision_world(collision_world)
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

void CharacterController::warp(const uicore::Vec3f &new_position, const uicore::Vec3f &new_velocity, bool new_is_flying)
{
	position = new_position;
	velocity = new_velocity;
	flying = new_is_flying;
}

void CharacterController::apply_impulse(const uicore::Vec3f &force)
{
	velocity += force / mass;
	begin_flying();
}

void CharacterController::thrust(const uicore::Vec2f &dir)
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

bool CharacterController::step_move(uicore::Vec3f move_vec)
{
	position.y += height * 0.5f + shape_margin;

	float vertical_sign = move_vec.y >= 0.0f ? 1.0f : -1.0f;

	bool hit_ceiling_or_floor = false;
	for (int iteration = 0; !hit_ceiling_or_floor && iteration < 3; iteration++)
	{
		Vec3f from_pos = position;
		Vec3f to_pos = position + move_vec;
		Quaternionf orientation;

		auto hits = collision_world->sweep_test_all_sorted(collision_shape, from_pos, orientation, to_pos, orientation, allowed_ccd, [](const Physics3DHit &result)
		{
			return (result.object->static_object() || result.object->kinematic_object()) && result.object->data<PlayerPawn>() == nullptr;
		});

		if (!hits.empty())
		{
			// Look for a floor/ceiling first as multiple faces might have contact at face edges
			float t = hits[0].fraction;
			for (const auto &hit : hits)
			{
				float dot = Vec3f::dot(Vec3f(0.0f, vertical_sign, 0.0f), hit.normal);
				if (dot < -acos_too_steep_slope || dot > acos_too_steep_slope)
				{
					position = hits[0].position;
					hit_ceiling_or_floor = true;
					break; // hit ceiling or floor
				}
			}

			// If we didn't hit a floor/ceiling we must have hit a steep wall, adjust movement vector and try again
			if (!hit_ceiling_or_floor)
			{
				float margin = allowed_ccd * 0.3f;
				Vec3f normal = hits[0].normal;
				move_vec = mix(move_vec, reflect(move_vec, normal, step_bounce), 1.0f - hits[0].fraction) + normal * margin;
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
		begin_flying();

	step_move(Vec3f(0.0f, step_height, 0.0f));
	land_impact = 0.0f;
}

void CharacterController::end_step_up()
{
	if (step_move(Vec3f(0.0f, -step_height, 0.0f)))
	{
		land_impact = std::max(land_impact, -velocity.y);
		if (flying)
		{
			// Absorb some of the speed if we land in a direction different than forward/back (for better impulse based landings)
			auto move_direction = Quaternionf::euler(0.0f, radians(rotation.dir), 0.0f);
			Vec3f move_vector = move_direction.rotate_vector(Vec3f(0.0f, 0.0f, 1.0f));
			velocity = mix(move_vector * Vec3f::dot(move_vector, velocity), velocity, 0.25f);
		}
		velocity.y = 0.0f;
		end_flying();
	}
}

void CharacterController::step_down()
{
	if (!flying)
	{
		if (step_move(Vec3f(0.0f, -step_height, 0.0f)))
		{
			land_impact = std::max(land_impact, -velocity.y);
			velocity.y = 0.0f;
			end_flying();
		}
		else
		{
			begin_flying();
		}
	}
}

void CharacterController::begin_flying()
{
	flying = true;
}

void CharacterController::end_flying()
{
	flying = false;
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
		auto hits = collision_world->sweep_test_all_sorted(collision_shape, from_pos, orientation, to_pos, orientation, allowed_ccd, [](const Physics3DHit &result)
		{
			return (result.object->static_object() || result.object->kinematic_object()) && result.object->data<PlayerPawn>() == nullptr;
		});
		for (const auto &hit : hits)
		{
			//if (Vec3f::dot(velocity, hit.normal) > 0)
			//	continue;

			position = hit.position;
			velocity = reflect(velocity, hit.normal, bounce);

			t = t + hit.fraction * (1.0f - t);

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

	auto move_direction = Quaternionf::euler(0.0f, radians(rotation.dir), 0.0f);
	Vec3f move_vector = move_direction.rotate_vector(Vec3f(thrust_vec.x, 0.0f, thrust_vec.y));

	if (flying)
	{
		float y = velocity.y;
		velocity.y = 0.0f;

		float fly_speed = velocity.length();

		Vec3f new_velocity = velocity + move_vector * acceleration * air_movement * tick_elapsed;
		float new_speed = new_velocity.length();
		new_velocity.normalize();

		velocity = new_velocity * std::min(std::max(fly_speed, run_speed * air_movement), new_speed);
		velocity.y = y;
	}
	else
	{
		float speed = velocity.length();
		velocity.normalize();
		velocity *= std::max(speed - acceleration * tick_elapsed, 0.0f);

		velocity += move_vector * acceleration * 2.0f * tick_elapsed;

		speed = std::min(velocity.length(), run_speed);
		velocity.normalize();
		velocity *= speed;
	}
}

Vec3f CharacterController::reflect(const Vec3f &ray, const Vec3f &normal, float amount)
{
	return ray - normal * ((1.0f + amount) * ray.dot(normal));
}
