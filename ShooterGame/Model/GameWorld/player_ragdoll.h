
#pragma once

#include "game_object.h"

class PlayerRagdoll : public GameObject
{
public:
	PlayerRagdoll(GameWorld *world, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation);
	~PlayerRagdoll();

	void tick() override;
	void frame(float time_elapsed, float interpolated_time) override;

private:
	enum PartName
	{
		part_torso,
		part_head,
		part_upper_arm_right,
		part_upper_arm_left,
		part_lower_arm_right,
		part_lower_arm_left,
		part_upper_leg_right,
		part_upper_leg_left,
		part_lower_leg_right,
		part_lower_leg_left,
		total_parts
	};

	enum JointName
	{
		joint_head,
		joint_torso_upper_leg_right,
		joint_torso_upper_leg_left,
		joint_torso_upper_arm_right,
		joint_torso_upper_arm_left,
		joint_upper_arm_right_lower_arm_right,
		joint_upper_arm_left_lower_arm_left,
		joint_upper_leg_right_lower_leg_right,
		joint_upper_leg_left_lower_leg_left,
		total_joints
	};

	void calc_constraint_location(JointName joint, PartName part_a, uicore::Vec3f pos_a, PartName part_b, uicore::Vec3f pos_b, uicore::Quaternionf rotation);

	SceneObjectPtr scene_box(const uicore::Vec3f &box_size);
	SceneObjectPtr scene_capsule(float radius, float height);

	std::shared_ptr<ModelData> create_box(const uicore::Vec3f &box_size);

	bool first_tick = true;
	float physics_timeout = 10.0f;

	SceneObjectPtr scene_object;
	SceneObjectPtr objects[total_parts];
	uicore::Vec3f prev_pos[total_parts], next_pos[total_parts];
	uicore::Quaternionf prev_orientation[total_parts], next_orientation[total_parts];

	Physics3DShapePtr shapes[total_parts];
	Physics3DObjectPtr parts[total_parts];
	float mass[total_parts];
	Physics3DConstraintPtr joints[total_joints];
	uicore::Vec3f joints_pos_a[total_joints];
	uicore::Vec3f joints_pos_b[total_joints];
	uicore::Quaternionf joints_rotate_a[total_joints];
	uicore::Quaternionf joints_rotate_b[total_joints];
};
