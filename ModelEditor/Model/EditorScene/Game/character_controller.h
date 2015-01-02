
#pragma once

class CollisionEntity
{
public:
	virtual ~CollisionEntity() { }
};

class EulerRotation
{
public:
	EulerRotation() { }
	EulerRotation(float dir, float up, float tilt = 0.0f) : dir(dir), up(up), tilt(tilt) { }

	clan::Quaternionf to_quaternionf() const { return clan::Quaternionf(up, dir, tilt, clan::angle_degrees, clan::order_YXZ); }

	static EulerRotation lerp(const EulerRotation &a, const EulerRotation &b, float t)
	{
		return EulerRotation(clan::mix(a.dir, b.dir, t), clan::mix(a.up, b.up, t), clan::mix(a.tilt, b.tilt, t));
	}

	float dir = 0.0f;
	float up = 0.0f;
	float tilt = 0.0f;
};

class CharacterController : public CollisionEntity
{
public:
	CharacterController(clan::Physics3DWorld collision_world);

	bool is_flying() const { return flying; }
	const clan::Vec3f &get_position() const { return position; }
	const EulerRotation &get_rotation() const { return rotation; }

	void set_mass(float mass);
	void set_size(float height, float radius);
	void set_max_speed(float max_speed);
	void set_air_resistance(float air_resistance);
	void set_air_movement(float air_movement);
	void set_gravity(float gravity);

	void warp(const clan::Vec3f &position, const EulerRotation &rotation, const clan::Vec3f &velocity);

	void apply_impulse(const clan::Vec3f &force);
	void thrust(const clan::Vec2f &dir);
	void look(const EulerRotation &rotation);

	void update(float tick_elapsed);

private:
	void update_shape();
	void apply_gravity(float tick_elapsed);
	void apply_air_resistance(float tick_elapsed);
	void apply_velocity(float tick_elapsed);
	void apply_thrust(float tick_elapsed);

	static clan::Vec3f reflect(const clan::Vec3f &ray, const clan::Vec3f &normal, float amount = 1.0f);

	clan::Physics3DWorld collision_world;
	clan::Physics3DShape collision_shape;
	clan::Physics3DSweepTest sweep_test;

	float allowed_ccd = 0.001f; // 1 millimeter

	float mass = 1.0f;
	float height = 1.8f;
	float radius = 0.5f;
	float max_speed = 2.0f;
	float air_resistance = 0.98f;
	float air_movement = 0.25f;
	float gravity = 4.0f;

	clan::Vec3f position;
	EulerRotation rotation;
	clan::Vec2f thrust_vec;

	clan::Vec3f velocity;
	bool flying = false;

	bool shape_modified = true;
};
