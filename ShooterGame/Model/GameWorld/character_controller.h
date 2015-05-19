
#pragma once

class CharacterController
{
public:
	CharacterController(clan::Physics3DWorld &world, float radius, float height, float step_height, float gravity);

	clan::Physics3DShape get_shape() const { return shape; }
	clan::Physics3DObject get_object() const { return object; }

	clan::Vec3f get_position() const;
	void set_position(const clan::Vec3f &position);

	clan::Vec3f get_fly_velocity(bool &out_flying) { out_flying = flying; return fly_velocity; }
	void set_fly_velocity(const clan::Vec3f &velocity, bool is_flying) { fly_velocity = velocity; flying = is_flying; }

	void move(const clan::Vec3f &velocity, float time_elapsed);
	void jump(const clan::Vec3f &jump_velocity);

private:
	void recover_from_penetration();
	bool try_move_to(const clan::Vec3f &target);
	void move_forward(clan::Vec3f target);
	bool move_vertical(clan::Vec3f target);

	static clan::Vec3f reflect(const clan::Vec3f &direction, const clan::Vec3f &normal);
	static clan::Vec3f parallel_component(const clan::Vec3f &direction, const clan::Vec3f &normal);
	static clan::Vec3f perpendicular_component(const clan::Vec3f &direction, const clan::Vec3f &normal);

	clan::Physics3DObject object;

	clan::Physics3DShape shape, shape2;
	float height_offset;
	clan::Vec3f position;
	float step_height;
	float gravity;
	clan::Vec3f fly_velocity;
	bool flying;

	float margin;

	clan::Physics3DContactTest contact_test;
	clan::Physics3DSweepTest sweep_test;

	float allowed_ccd_penetration;

	const float air_speed = 0.20f;
};
