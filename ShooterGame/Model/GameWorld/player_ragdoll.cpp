
#include "precomp.h"
#include "player_ragdoll.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

PlayerRagdoll::PlayerRagdoll(GameWorld *world, const Vec3f &pos, const Quaternionf &orientation) : GameObject(world)
{
	//auto model = SceneModel::create(game_world()->client()->scene(), "Models/Kachujin/Kachujin.cmodel");
	//scene_object = SceneObject::create(game_world()->client()->scene(), model);

	float scale = 0.55f;
	float margin = 0.1f * scale;

	Vec3f sizes[total_parts];
	sizes[part_torso] = Vec3f(0.35f, 0.625f, 0.275f) * scale;
	sizes[part_head] = Vec3f(0.175f, 0.2f, 0.2f) * scale;
	sizes[part_upper_arm_right] = Vec3f(0.1f, 0.55f, 0.0f) * scale;
	sizes[part_upper_arm_left] = Vec3f(0.1f, 0.55f, 0.0f) * scale;
	sizes[part_lower_arm_right] = Vec3f(0.125f, 0.8f, 0.0f) * scale;
	sizes[part_lower_arm_left] = Vec3f(0.125f, 0.8f, 0.0f) * scale;
	sizes[part_upper_leg_right] = Vec3f(0.15f, 0.9f, 0.0f) * scale;
	sizes[part_upper_leg_left] = Vec3f(0.15f, 0.9f, 0.0f) * scale;
	sizes[part_lower_leg_right] = Vec3f(0.125f, 0.95f, 0.0f) * scale;
	sizes[part_lower_leg_left] = Vec3f(0.125f, 0.95f, 0.0f) * scale;

	objects[part_torso] = scene_box(sizes[part_torso]);
	objects[part_head] = scene_box(sizes[part_head]);
	objects[part_upper_arm_right] = scene_capsule(sizes[part_upper_arm_right].x, sizes[part_upper_arm_right].y);
	objects[part_upper_arm_left] = scene_capsule(sizes[part_upper_arm_left].x, sizes[part_upper_arm_left].y);
	objects[part_lower_arm_right] = scene_capsule(sizes[part_lower_arm_right].x, sizes[part_lower_arm_right].y);
	objects[part_lower_arm_left] = scene_capsule(sizes[part_lower_arm_left].x, sizes[part_lower_arm_left].y);
	objects[part_upper_leg_right] = scene_capsule(sizes[part_upper_leg_right].x, sizes[part_upper_leg_right].y);
	objects[part_upper_leg_left] = scene_capsule(sizes[part_upper_leg_left].x, sizes[part_upper_leg_left].y);
	objects[part_lower_leg_right] = scene_capsule(sizes[part_lower_leg_right].x, sizes[part_lower_leg_right].y);
	objects[part_lower_leg_left] = scene_capsule(sizes[part_lower_leg_left].x, sizes[part_lower_leg_left].y);

	shapes[part_torso] = Physics3DShape::box(sizes[part_torso] - margin);
	shapes[part_head] = Physics3DShape::box(sizes[part_head] - margin);
	shapes[part_upper_arm_right] = Physics3DShape::capsule(sizes[part_upper_arm_right].x, sizes[part_upper_arm_right].y - sizes[part_upper_arm_right].x - margin);
	shapes[part_upper_arm_left] = Physics3DShape::capsule(sizes[part_upper_arm_left].x, sizes[part_upper_arm_left].y - sizes[part_upper_arm_left].x - margin);
	shapes[part_lower_arm_right] = Physics3DShape::capsule(sizes[part_lower_arm_right].x, sizes[part_lower_arm_right].y - sizes[part_lower_arm_right].x - margin);
	shapes[part_lower_arm_left] = Physics3DShape::capsule(sizes[part_lower_arm_left].x, sizes[part_lower_arm_left].y - sizes[part_lower_arm_left].x - margin);
	shapes[part_upper_leg_right] = Physics3DShape::capsule(sizes[part_upper_leg_right].x, sizes[part_upper_leg_right].y - sizes[part_upper_leg_right].x - margin);
	shapes[part_upper_leg_left] = Physics3DShape::capsule(sizes[part_upper_leg_left].x, sizes[part_upper_leg_left].y - sizes[part_upper_leg_left].x - margin);
	shapes[part_lower_leg_right] = Physics3DShape::capsule(sizes[part_lower_leg_right].x, sizes[part_lower_leg_right].y - sizes[part_lower_leg_right].x - margin);
	shapes[part_lower_leg_left] = Physics3DShape::capsule(sizes[part_lower_leg_left].x, sizes[part_lower_leg_left].y - sizes[part_lower_leg_left].x - margin);

	mass[part_torso] = sizes[part_torso].x * sizes[part_torso].y * sizes[part_torso].z * 16.0f;
	mass[part_head] = sizes[part_head].x * sizes[part_head].y * sizes[part_head].z * 16.0f;
	mass[part_upper_arm_right] = sizes[part_upper_arm_right].x * sizes[part_upper_arm_right].y * sizes[part_upper_arm_right].x * 8.0f;
	mass[part_upper_arm_left] = sizes[part_upper_arm_left].x * sizes[part_upper_arm_left].y * sizes[part_upper_arm_left].x * 8.0f;
	mass[part_lower_arm_right] = sizes[part_lower_arm_right].x * sizes[part_lower_arm_right].y * sizes[part_lower_arm_right].x * 8.0f;
	mass[part_lower_arm_left] = sizes[part_lower_arm_left].x * sizes[part_lower_arm_left].y * sizes[part_lower_arm_left].x * 8.0f;
	mass[part_upper_leg_right] = sizes[part_upper_leg_right].x * sizes[part_upper_leg_right].y * sizes[part_upper_leg_right].x * 8.0f;
	mass[part_upper_leg_left] = sizes[part_upper_leg_left].x * sizes[part_upper_leg_left].y * sizes[part_upper_leg_left].x * 8.0f;
	mass[part_lower_leg_right] = sizes[part_lower_leg_right].x * sizes[part_lower_leg_right].y * sizes[part_lower_leg_right].x * 8.0f;
	mass[part_lower_leg_left] = sizes[part_lower_leg_left].x * sizes[part_lower_leg_left].y * sizes[part_lower_leg_left].x * 8.0f;

	for (int i = 0; i < total_parts; i++)
	{
		objects[i]->set_position(pos);
		objects[i]->set_orientation(orientation);
		parts[i] = Physics3DObject::rigid_body(game_world()->dynamic_collision(), shapes[i], mass[i], pos, orientation);
	}

	calc_constraint_location(joint_head, part_torso, Vec3f(0, sizes[part_torso].y, 0), part_head, Vec3f(0, -sizes[part_head].y, 0), Quaternionf(1, 0, 0, 0));
	calc_constraint_location(joint_upper_arm_left_lower_arm_left, part_upper_arm_left, Vec3f(0, sizes[part_upper_arm_left].y * 0.5f, 0), part_lower_arm_left, Vec3f(0, -sizes[part_lower_arm_left].y * 0.5f, 0), Quaternionf(0.70f, -0.0f, 0.7f, -0.0f));
	calc_constraint_location(joint_upper_arm_right_lower_arm_right, part_upper_arm_right, Vec3f(0, sizes[part_upper_arm_left].y * 0.5f, 0), part_lower_arm_right, Vec3f(0, -sizes[part_lower_arm_left].y * 0.5f, 0), Quaternionf(0.70f, -0.0f, 0.7f, -0.0f));
	calc_constraint_location(joint_upper_leg_left_lower_leg_left, part_upper_leg_left, Vec3f(0, sizes[part_upper_leg_left].y * 0.5f, 0), part_lower_arm_left, Vec3f(0, -sizes[part_lower_leg_left].y * 0.5f, 0), Quaternionf(0.70f, -0.0f, 0.70f, -0.0f));
	calc_constraint_location(joint_upper_leg_right_lower_leg_right, part_upper_leg_right, Vec3f(0, sizes[part_upper_leg_right].y * 0.5f, 0), part_lower_arm_right, Vec3f(0, -sizes[part_lower_leg_right].y * 0.5f, 0), Quaternionf(0.70f, -0.0f, 0.70f, -0.0f));
	calc_constraint_location(joint_torso_upper_leg_left, part_torso, Vec3f(sizes[part_torso].x - sizes[part_upper_leg_left].x, -sizes[part_torso].y, 0), part_upper_leg_left, Vec3f(0, -sizes[part_upper_leg_left].y * 0.5f, 0), Quaternionf(0.38f, 0.0f, 0.0f, 0.92f));
	calc_constraint_location(joint_torso_upper_leg_right, part_torso, Vec3f(-sizes[part_torso].x + sizes[part_upper_leg_right].x, -sizes[part_torso].y, 0), part_upper_leg_right, Vec3f(0, -sizes[part_upper_leg_right].y * 0.5f, 0), Quaternionf(0.38f, 0.0f, 0.0f, 0.92f));
	calc_constraint_location(joint_torso_upper_arm_left, part_torso, Vec3f(sizes[part_torso].x, sizes[part_torso].y - sizes[part_upper_arm_left].x, 0), part_upper_arm_left, Vec3f(0, -sizes[part_upper_arm_left].y * 0.5f, 0), Quaternionf(0, 0, 0, 1));
	calc_constraint_location(joint_torso_upper_arm_right, part_torso, Vec3f(-sizes[part_torso].x, sizes[part_torso].y - sizes[part_upper_arm_right].x, 0), part_upper_arm_right, Vec3f(0, -sizes[part_upper_arm_right].y * 0.5f, 0), Quaternionf(0, 0, 0, 1));
	
	joints[joint_head] = Physics3DConstraint::cone_twist(game_world()->dynamic_collision(), parts[part_torso], parts[part_head], joints_pos_a[joint_head], joints_pos_b[joint_head], joints_rotate_a[joint_head], joints_rotate_b[joint_head]);
	//joints[joint_head]->set_cone_twist_limit(0.0f, 0.7f, 0.5f);

	joints[joint_upper_arm_right_lower_arm_right] = Physics3DConstraint::hinge(game_world()->dynamic_collision(), parts[part_upper_arm_right], parts[part_lower_arm_right], joints_pos_a[joint_upper_arm_right_lower_arm_right], joints_pos_b[joint_upper_arm_right_lower_arm_right], joints_rotate_a[joint_upper_arm_right_lower_arm_right], joints_rotate_b[joint_upper_arm_right_lower_arm_right]);
	joints[joint_upper_arm_right_lower_arm_right]->set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_arm_left_lower_arm_left] = Physics3DConstraint::hinge(game_world()->dynamic_collision(), parts[part_upper_arm_left], parts[part_lower_arm_left], joints_pos_a[joint_upper_arm_left_lower_arm_left], joints_pos_b[joint_upper_arm_left_lower_arm_left], joints_rotate_a[joint_upper_arm_left_lower_arm_left], joints_rotate_b[joint_upper_arm_left_lower_arm_left]);
	joints[joint_upper_arm_left_lower_arm_left]->set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_leg_right_lower_leg_right] = Physics3DConstraint::hinge(game_world()->dynamic_collision(), parts[part_upper_leg_right], parts[part_lower_leg_right], joints_pos_a[joint_upper_leg_right_lower_leg_right], joints_pos_b[joint_upper_leg_right_lower_leg_right], joints_rotate_a[joint_upper_leg_right_lower_leg_right], joints_rotate_b[joint_upper_leg_right_lower_leg_right]);
	joints[joint_upper_leg_right_lower_leg_right]->set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_leg_left_lower_leg_left] = Physics3DConstraint::hinge(game_world()->dynamic_collision(), parts[part_upper_leg_left], parts[part_lower_leg_left], joints_pos_a[joint_upper_leg_left_lower_leg_left], joints_pos_b[joint_upper_leg_left_lower_leg_left], joints_rotate_a[joint_upper_leg_left_lower_leg_left], joints_rotate_b[joint_upper_leg_left_lower_leg_left]);
	joints[joint_upper_leg_left_lower_leg_left]->set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_torso_upper_leg_right] = Physics3DConstraint::cone_twist(game_world()->dynamic_collision(), parts[part_torso], parts[part_upper_leg_right], joints_pos_a[joint_torso_upper_leg_right], joints_pos_b[joint_torso_upper_leg_right], joints_rotate_a[joint_torso_upper_leg_right], joints_rotate_b[joint_torso_upper_leg_right]);
	//joints[joint_torso_upper_leg_right]->set_cone_twist_limit(0.0f, PI_F / 4.0f, PI_F / 4.0f);

	joints[joint_torso_upper_leg_left] = Physics3DConstraint::cone_twist(game_world()->dynamic_collision(), parts[part_torso], parts[part_upper_leg_left], joints_pos_a[joint_torso_upper_leg_left], joints_pos_b[joint_torso_upper_leg_left], joints_rotate_a[joint_torso_upper_leg_left], joints_rotate_b[joint_torso_upper_leg_left]);
	//joints[joint_torso_upper_leg_left]->set_cone_twist_limit(0.0f, PI_F / 4.0f, PI_F / 4.0f);

	joints[joint_torso_upper_arm_right] = Physics3DConstraint::cone_twist(game_world()->dynamic_collision(), parts[part_torso], parts[part_upper_arm_right], joints_pos_a[joint_torso_upper_arm_right], joints_pos_b[joint_torso_upper_arm_right], joints_rotate_a[joint_torso_upper_arm_right], joints_rotate_b[joint_torso_upper_arm_right]);
	//joints[joint_torso_upper_arm_right]->set_cone_twist_limit(0.0f, PI_F / 2.0f, PI_F / 2.0f);

	joints[joint_torso_upper_arm_left] = Physics3DConstraint::cone_twist(game_world()->dynamic_collision(), parts[part_torso], parts[part_upper_arm_left], joints_pos_a[joint_torso_upper_arm_left], joints_pos_b[joint_torso_upper_arm_left], joints_rotate_a[joint_torso_upper_arm_left], joints_rotate_b[joint_torso_upper_arm_left]);
	//joints[joint_torso_upper_arm_left]->set_cone_twist_limit(0.0f, PI_F / 2.0f, PI_F / 2.0f);
}

PlayerRagdoll::~PlayerRagdoll()
{
}

SceneObjectPtr PlayerRagdoll::scene_box(const uicore::Vec3f &box_size)
{
	auto &model = game_world()->client()->box_models()[box_size];
	if (!model)
		model = SceneModel::create(game_world()->client()->scene(), create_box(box_size));
	return SceneObject::create(game_world()->client()->scene(), model);
}

SceneObjectPtr PlayerRagdoll::scene_capsule(float radius, float height)
{
	auto &model = game_world()->client()->capsule_models()[Vec2f(radius, height)];
	if (!model)
		model = SceneModel::create(game_world()->client()->scene(), create_box(Vec3f(radius, height * 0.5f, radius)));
	return SceneObject::create(game_world()->client()->scene(), model);
}

void PlayerRagdoll::calc_constraint_location(JointName joint, PartName part_a, Vec3f pos_a, PartName part_b, Vec3f pos_b, uicore::Quaternionf rotation)
{
	/*
	auto part_a_to_world = Mat4f::translate(parts[part_a]->position()) * parts[part_a]->orientation().to_matrix();
	auto part_b_to_world = Mat4f::translate(parts[part_b]->position()) * parts[part_b]->orientation().to_matrix();
	auto world_to_part_a = Mat4f::inverse(part_a_to_world);
	auto world_to_part_b = Mat4f::inverse(part_b_to_world);

	auto pos_a_in_world = part_a_to_world * Vec4f(pos_a, 1.0f);
	auto pos_b_in_world = part_b_to_world * Vec4f(pos_b, 1.0f);

	auto joint_in_world = (pos_a_in_world + pos_b_in_world) * 0.5f;
	*/

	joints_pos_a[joint] = pos_a;// Vec3f(world_to_part_a * joint_in_world);
	joints_pos_b[joint] = pos_b;// Vec3f(world_to_part_b * joint_in_world);
	joints_rotate_a[joint] = rotation;
	joints_rotate_b[joint] = rotation;
}

void PlayerRagdoll::tick()
{
	physics_timeout = std::max(physics_timeout - time_elapsed(), 0.0f);
	if (physics_timeout == 0.0f && parts[0])
	{
		for (int i = 0; i < total_joints; i++)
			joints[i].reset();
		for (int i = 0; i < total_parts; i++)
			parts[i].reset();
	}

	for (int i = 0; i < total_parts; i++)
	{
		prev_pos[i] = next_pos[i];
		prev_orientation[i] = next_orientation[i];
		if (parts[i])
		{
			next_pos[i] = parts[i]->position();
			next_orientation[i] = parts[i]->orientation();
		}
	}

	if (first_tick)
	{
		for (int i = 0; i < total_parts; i++)
		{
			prev_pos[i] = next_pos[i];
			prev_orientation[i] = next_orientation[i];
		}
		first_tick = false;
	}
}

void PlayerRagdoll::frame()
{
	/*
	std::vector<Vec3f> positions;
	std::vector<Quaternionf> rotations;
	positions.resize(30);
	rotations.resize(30);
	scene_object->set_bone_positions(positions);
	scene_object->set_bone_orientations(rotations);
	scene_object->set_position(mix(prev_pos[part_torso], next_pos[part_torso], interpolated_time) + Vec3f(0.0f, 4.0f, 0.0f));
	scene_object->set_orientation(Quaternionf::slerp(prev_orientation[part_torso], next_orientation[part_torso], interpolated_time));
	*/

	for (int i = 0; i < total_parts; i++)
	{
		objects[i]->update(frame_time_elapsed());
		objects[i]->set_position(mix(prev_pos[i], next_pos[i], frame_interpolated_time()));
		objects[i]->set_orientation(Quaternionf::slerp(prev_orientation[i], next_orientation[i], frame_interpolated_time()));
	}
}

std::shared_ptr<ModelData> PlayerRagdoll::create_box(const Vec3f &box_size)
{
	std::shared_ptr<ModelData> model_data(new ModelData());

	model_data->aabb_min = -box_size;
	model_data->aabb_max = box_size;

	model_data->animations.resize(3);
	model_data->animations[0].name = "default";
	model_data->animations[1].name = "up";
	model_data->animations[2].name = "down";

	model_data->meshes.resize(1);
	model_data->meshes[0].channels.resize(4);

	Vec3f normal[6] =
	{
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, -1.0f)
	};

	Vec3f tangent[6] =
	{
		Vec3f(1.0, 0.0f, 0.0f),
		Vec3f(1.0, 0.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f)
	};

	for (int i = 0; i < 6; i++)
	{
		Vec3f bitangent = Vec3f::cross(normal[i], tangent[i]);

		model_data->meshes[0].vertices.push_back((normal[i] - tangent[i] - bitangent) * box_size);
		model_data->meshes[0].vertices.push_back((normal[i] + tangent[i] - bitangent) * box_size);
		model_data->meshes[0].vertices.push_back((normal[i] - tangent[i] + bitangent) * box_size);
		model_data->meshes[0].vertices.push_back((normal[i] + tangent[i] + bitangent) * box_size);

		for (int j = 0; j < 4; j++)
		{
			model_data->meshes[0].normals.push_back(normal[i]);
		}

		for (int j = 0; j < 4; j++)
		{
			model_data->meshes[0].channels[j].push_back(Vec2f(0.0f, 0.0f));
			model_data->meshes[0].channels[j].push_back(Vec2f(1.0f, 0.0f));
			model_data->meshes[0].channels[j].push_back(Vec2f(0.0f, 1.0f));
			model_data->meshes[0].channels[j].push_back(Vec2f(1.0f, 1.0f));
		}

		model_data->meshes[0].elements.push_back(i * 4 + 0);
		model_data->meshes[0].elements.push_back(i * 4 + 1);
		model_data->meshes[0].elements.push_back(i * 4 + 2);
		model_data->meshes[0].elements.push_back(i * 4 + 3);
		model_data->meshes[0].elements.push_back(i * 4 + 2);
		model_data->meshes[0].elements.push_back(i * 4 + 1);
	}

	ModelDataDrawRange range;
	range.start_element = 0;
	range.num_elements = 36;
	range.ambient.set_single_value(Vec3f(1.0f));
	range.diffuse.set_single_value(Vec3f(1.0f));
	range.specular.set_single_value(Vec3f(1.0f));
	range.glossiness.set_single_value(20.0f);
	range.specular_level.set_single_value(25.0f);

	range.self_illumination_amount.timelines.resize(1);
	range.self_illumination_amount.timelines[0].timestamps.push_back(0.0f);
	range.self_illumination_amount.timelines[0].values.push_back(0.0);

	range.self_illumination.timelines.resize(1);
	range.self_illumination.timelines[0].timestamps.push_back(0.0f);
	range.self_illumination.timelines[0].values.push_back(Vec3f());

	model_data->meshes[0].draw_ranges.push_back(range);

	return model_data;
}
