
#pragma once

#include "game_object.h"

class PlayerRagdoll : public GameObject
{
public:
	PlayerRagdoll(GameWorld *world, const uicore::Vec3f &pos, const uicore::Quaternionf &orientation);
	~PlayerRagdoll();

	void tick(const GameTick &tick) override;
	void frame(float time_elapsed, float interpolated_time) override;

protected:
	std::shared_ptr<uicore::ModelData> create_box(const uicore::Vec3f &box_size);

	uicore::SceneObject scene_object1;
	uicore::SceneObject scene_object2;

	Physics3DConstraint hinge_constraint;
	Physics3DObject physics_object1;
	Physics3DObject physics_object2;

	uicore::Vec3f prev_pos1, next_pos1;
	uicore::Vec3f prev_pos2, next_pos2;
	uicore::Quaternionf prev_orientation1, next_orientation1;
	uicore::Quaternionf prev_orientation2, next_orientation2;

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

	Physics3DShape shapes[total_parts];
	Physics3DObject parts[total_parts];
	Physics3DConstraint joints[total_joints];
};
