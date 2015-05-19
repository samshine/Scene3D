
#pragma once

#include "game_object.h"

class PlayerRagdoll : public GameObject
{
public:
	PlayerRagdoll(GameWorld *world, const clan::Vec3f &pos, const clan::Quaternionf &orientation);
	~PlayerRagdoll();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

protected:
	std::shared_ptr<clan::ModelData> create_box(const clan::Vec3f &box_size);

	clan::SceneObject scene_object1;
	clan::SceneObject scene_object2;

	clan::Physics3DConstraint hinge_constraint;
	clan::Physics3DObject physics_object1;
	clan::Physics3DObject physics_object2;

	clan::Vec3f prev_pos1, next_pos1;
	clan::Vec3f prev_pos2, next_pos2;
	clan::Quaternionf prev_orientation1, next_orientation1;
	clan::Quaternionf prev_orientation2, next_orientation2;

	bool first_tick = true;

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

	clan::Physics3DShape shapes[total_parts];
	clan::Physics3DObject parts[total_parts];
	clan::Physics3DConstraint joints[total_joints];
};
