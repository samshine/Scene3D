
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

	uicore::Quaternionf to_quaternionf() const { return uicore::Quaternionf(up, dir, tilt, uicore::angle_degrees, uicore::order_YXZ); }

	static EulerRotation lerp(const EulerRotation &a, const EulerRotation &b, float t)
	{
		return EulerRotation(uicore::mix(a.dir, b.dir, t), uicore::mix(a.up, b.up, t), uicore::mix(a.tilt, b.tilt, t));
	}

	float dir = 0.0f;
	float up = 0.0f;
	float tilt = 0.0f;
};

class CharacterController : public CollisionEntity
{
public:
	CharacterController(const Physics3DWorldPtr &collision_world);

	bool is_flying() const { return flying; }
	const uicore::Vec3f &get_position() const { return position; }
	const EulerRotation &get_rotation() const { return rotation; }
	const uicore::Vec3f &get_velocity() const { return velocity; }

	float get_gravity() const { return gravity; }
	float get_height() const { return height; }
	float get_radius() const { return radius; }
	float get_step_height() const { return step_height; }
	float get_mass() const { return mass; }
	float get_acceleration() const { return acceleration; }
	float get_run_speed() const { return run_speed; }
	float get_friction() const { return friction; }
	float get_air_resistance() const { return air_resistance; }
	float get_air_movement() const { return air_movement; }
	float get_bounce() const { return bounce; }

	void set_gravity(float gravity);
	void set_height(float height);
	void set_radius(float radius);
	void set_step_height(float step_height);
	void set_mass(float mass);
	void set_acceleration(float acceleration);
	void set_run_speed(float run_speed);
	void set_friction(float friction);
	void set_air_resistance(float air_resistance);
	void set_air_movement(float air_movement);
	void set_bounce(float bounce);

	void warp(const uicore::Vec3f &position, const uicore::Vec3f &velocity, bool is_flying);

	void apply_impulse(const uicore::Vec3f &force);
	void thrust(const uicore::Vec2f &dir);
	void look(const EulerRotation &rotation);

	void update(float tick_elapsed);

	float get_land_impact() const { return land_impact; }

private:
	void update_shape();
	void begin_step_up();
	void end_step_up();
	void step_down();
	bool step_move(uicore::Vec3f move_vec);
	void apply_gravity(float tick_elapsed);
	void apply_velocity(float tick_elapsed);
	void apply_thrust(float tick_elapsed);

	void begin_flying();
	void end_flying();

	static uicore::Vec3f reflect(const uicore::Vec3f &ray, const uicore::Vec3f &normal, float amount = 1.0f);

	Physics3DWorldPtr collision_world;
	Physics3DShapePtr collision_shape;
	Physics3DSweepTestPtr sweep_test;

	float allowed_ccd = 0.001f; // 1 millimeter
	float acos_too_steep_slope = 0.70f; // cos(45 deg)
	float shape_margin = 0.04f;

	float gravity = 9.8f * 1.6f;
	float height = 1.8f;
	float radius = 0.5f;
	float mass = 78.0f;
	float acceleration = 38.0f;
	float run_speed = 9.0f;
	float friction = 0.0f;
	float air_resistance = 0.3f;
	float air_movement = 0.35f;
	float bounce = 0.15f;
	float step_height = 0.25f;
	float step_bounce = 0.02f;

	float land_impact = 0.0f;

	uicore::Vec3f position;
	EulerRotation rotation;

	uicore::Vec3f velocity;
	uicore::Vec2f thrust_vec;
	bool flying = false;

	bool shape_modified = true;
};
