
#pragma once

class OldCharacterController
{
public:
	OldCharacterController(uicore::Physics3DWorld &world, float radius, float height, float step_height, float gravity);

	uicore::Physics3DShape get_shape() const { return shape; }
	uicore::Physics3DObject get_object() const { return object; }

	uicore::Vec3f get_position() const;
	void set_position(const uicore::Vec3f &position);

	uicore::Vec3f get_fly_velocity(bool &out_flying) { out_flying = flying; return fly_velocity; }
	void set_fly_velocity(const uicore::Vec3f &velocity, bool is_flying) { fly_velocity = velocity; flying = is_flying; }

	void move(const uicore::Vec3f &velocity, float time_elapsed);
	void jump(const uicore::Vec3f &jump_velocity);

private:
	void recover_from_penetration();
	bool try_move_to(const uicore::Vec3f &target);
	void move_forward(uicore::Vec3f target);
	bool move_vertical(uicore::Vec3f target);

	static uicore::Vec3f reflect(const uicore::Vec3f &direction, const uicore::Vec3f &normal);
	static uicore::Vec3f parallel_component(const uicore::Vec3f &direction, const uicore::Vec3f &normal);
	static uicore::Vec3f perpendicular_component(const uicore::Vec3f &direction, const uicore::Vec3f &normal);

	uicore::Physics3DObject object;

	uicore::Physics3DShape shape, shape2;
	float height_offset;
	uicore::Vec3f position;
	float step_height;
	float gravity;
	uicore::Vec3f fly_velocity;
	bool flying;

	float margin;

	uicore::Physics3DContactTest contact_test;
	uicore::Physics3DSweepTest sweep_test;

	float allowed_ccd_penetration;

	const float air_speed = 0.20f;
};
