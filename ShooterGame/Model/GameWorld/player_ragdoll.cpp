
#include "precomp.h"
#include "player_ragdoll.h"
#include "game_world.h"
#include "collision_game_object.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

PlayerRagdoll::PlayerRagdoll(GameWorld *world, const Vec3f &pos, const Quaternionf &orientation) : GameObject(world)
{
	objects[part_torso] = scene_box(Vec3f(0.7f, 1.25f, 0.55f));
	objects[part_head] = scene_box(Vec3f(0.35f, 0.4f, 0.4f));
	objects[part_upper_arm_right] = scene_capsule(0.2f, 1.1f);
	objects[part_upper_arm_left] = scene_capsule(0.2f, 1.1f);
	objects[part_lower_arm_right] = scene_capsule(0.25f, 1.6f);
	objects[part_lower_arm_left] = scene_capsule(0.25f, 1.6f);
	objects[part_upper_leg_right] = scene_capsule(0.3f, 1.8f);
	objects[part_upper_leg_left] = scene_capsule(0.3f, 1.8f);
	objects[part_lower_leg_right] = scene_capsule(0.25f, 1.9f);
	objects[part_lower_leg_left] = scene_capsule(0.25f, 1.9f);

	shapes[part_torso] = Physics3DShape::box(Vec3f(0.7f, 1.25f, 0.55f));
	shapes[part_head] = Physics3DShape::box(Vec3f(0.35f, 0.4f, 0.4f));
	shapes[part_upper_arm_right] = Physics3DShape::capsule(0.2f, 1.1f);
	shapes[part_upper_arm_left] = Physics3DShape::capsule(0.2f, 1.1f);
	shapes[part_lower_arm_right] = Physics3DShape::capsule(0.25f, 1.6f);
	shapes[part_lower_arm_left] = Physics3DShape::capsule(0.25f, 1.6f);
	shapes[part_upper_leg_right] = Physics3DShape::capsule(0.3f, 1.8f);
	shapes[part_upper_leg_left] = Physics3DShape::capsule(0.3f, 1.8f);
	shapes[part_lower_leg_right] = Physics3DShape::capsule(0.25f, 1.9f);
	shapes[part_lower_leg_left] = Physics3DShape::capsule(0.25f, 1.9f);

	mass[part_torso] = 0.7f * 1.25f * 0.55f * 8.0f;
	mass[part_head] = 0.35f * 0.4f * 0.4f * 8.0f;
	mass[part_upper_arm_right] = 0.2f * 1.1f * 0.2f * 4.0f;
	mass[part_upper_arm_left] = 0.2f * 1.1f * 0.2f * 4.0f;
	mass[part_lower_arm_right] = 0.25f * 1.6f * 0.25f * 4.0f;
	mass[part_lower_arm_left] = 0.25f * 1.6f * 0.25f * 4.0f;
	mass[part_upper_leg_right] = 0.3f * 1.8f * 0.3f * 4.0f;
	mass[part_upper_leg_left] = 0.3f * 1.8f * 0.3f * 4.0f;
	mass[part_lower_leg_right] = 0.25f * 1.9f * 0.25f * 4.0f;
	mass[part_lower_leg_left] = 0.25f * 1.9f * 0.25f * 4.0f;

	for (int i = 0; i < total_parts; i++)
	{
		objects[i]->set_position(pos);
		objects[i]->set_orientation(orientation);
		parts[i] = Physics3DObject::rigid_body(world->collision, shapes[i], mass[i], pos, orientation);
	}

	calc_constraint_location(joint_head, part_torso, Vec3f(0, 1.25f, 0), part_head, Vec3f(0, -0.4f, 0), Quaternionf(1, 0, 0, 0));
	calc_constraint_location(joint_upper_arm_right_lower_arm_right, part_upper_arm_left, Vec3f(0, 0.55f, 0), part_lower_arm_left, Vec3f(0, -0.8f, 0), Quaternionf(0.70f, -0.0f, 0.7f, -0.0f));
	calc_constraint_location(joint_upper_arm_left_lower_arm_left, part_upper_arm_right, Vec3f(0, 0.55f, 0), part_lower_arm_right, Vec3f(0, -0.8f, 0), Quaternionf(0.70f, -0.0f, 0.7f, -0.0f));
	calc_constraint_location(joint_upper_leg_left_lower_leg_left, part_upper_leg_left, Vec3f(0, 0.9f, 0), part_lower_leg_left, Vec3f(0, -0.95f, 0), Quaternionf(0.70f, -0.0f, 0.70f, -0.0f));
	calc_constraint_location(joint_upper_leg_right_lower_leg_right, part_upper_leg_right, Vec3f(0, 0.9f, 0), part_lower_leg_right, Vec3f(0, -0.95f, 0), Quaternionf(0.70f, -0.0f, 0.70f, -0.0f));
	calc_constraint_location(joint_torso_upper_leg_left, part_torso, Vec3f(0.4f, -1.25, 0), part_upper_leg_left, Vec3f(0, -0.9f, 0), Quaternionf(0.38f, 0.0f, 0.0f, 0.92f));
	calc_constraint_location(joint_torso_upper_leg_right, part_torso, Vec3f(-0.4f, -1.25, 0), part_upper_leg_right, Vec3f(0, -0.9f, 0), Quaternionf(0.38f, 0.0f, 0.0f, 0.92f));
	calc_constraint_location(joint_torso_upper_arm_right, part_torso, Vec3f(-0.7f, 1.1f, 0), part_upper_arm_right, Vec3f(0, -0.55f, 0), Quaternionf(0, 0, 0, 1));
	calc_constraint_location(joint_torso_upper_arm_left, part_torso, Vec3f(0.7f, 1.1f, 0), part_upper_arm_left, Vec3f(0, -0.55f, 0), Quaternionf(0, 0, 0, 1));
	
	joints[joint_head] = Physics3DConstraint::cone_twist(world->collision, parts[part_torso], parts[part_head], joints_pos_a[joint_head], joints_pos_b[joint_head], joints_rotate_a[joint_head], joints_rotate_b[joint_head]);
	joints[joint_head]->set_cone_twist_limit(0.7f, 0.5f, 0.0f);

	joints[joint_upper_arm_right_lower_arm_right] = Physics3DConstraint::hinge(world->collision, parts[part_upper_arm_right], parts[part_lower_arm_right], joints_pos_a[joint_upper_arm_right_lower_arm_right], joints_pos_b[joint_upper_arm_right_lower_arm_right], joints_rotate_a[joint_upper_arm_right_lower_arm_right], joints_rotate_b[joint_upper_arm_right_lower_arm_right]);
	joints[joint_upper_arm_right_lower_arm_right]->set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_arm_left_lower_arm_left] = Physics3DConstraint::hinge(world->collision, parts[part_upper_arm_left], parts[part_lower_arm_left], joints_pos_a[joint_upper_arm_left_lower_arm_left], joints_pos_b[joint_upper_arm_left_lower_arm_left], joints_rotate_a[joint_upper_arm_left_lower_arm_left], joints_rotate_b[joint_upper_arm_left_lower_arm_left]);
	joints[joint_upper_arm_left_lower_arm_left]->set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_leg_right_lower_leg_right] = Physics3DConstraint::hinge(world->collision, parts[part_upper_leg_right], parts[part_lower_leg_right], joints_pos_a[joint_upper_leg_right_lower_leg_right], joints_pos_b[joint_upper_leg_right_lower_leg_right], joints_rotate_a[joint_upper_leg_right_lower_leg_right], joints_rotate_b[joint_upper_leg_right_lower_leg_right]);
	joints[joint_upper_leg_right_lower_leg_right]->set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_leg_left_lower_leg_left] = Physics3DConstraint::hinge(world->collision, parts[part_upper_leg_left], parts[part_lower_leg_left], joints_pos_a[joint_upper_leg_left_lower_leg_left], joints_pos_b[joint_upper_leg_left_lower_leg_left], joints_rotate_a[joint_upper_leg_left_lower_leg_left], joints_rotate_b[joint_upper_leg_left_lower_leg_left]);
	joints[joint_upper_leg_left_lower_leg_left]->set_hinge_limit(-PI_F / 2.0f, 0);
	
	joints[joint_torso_upper_leg_right] = Physics3DConstraint::cone_twist(world->collision, parts[part_torso], parts[part_upper_leg_right], joints_pos_a[joint_torso_upper_leg_right], joints_pos_b[joint_torso_upper_leg_right], joints_rotate_a[joint_torso_upper_leg_right], joints_rotate_b[joint_torso_upper_leg_right]);
	joints[joint_torso_upper_leg_right]->set_cone_twist_limit(PI_F / 4.0f, PI_F / 4.0f, 0.0f);

	joints[joint_torso_upper_leg_left] = Physics3DConstraint::cone_twist(world->collision, parts[part_torso], parts[part_upper_leg_left], joints_pos_a[joint_torso_upper_leg_left], joints_pos_b[joint_torso_upper_leg_left], joints_rotate_a[joint_torso_upper_leg_left], joints_rotate_b[joint_torso_upper_leg_left]);
	joints[joint_torso_upper_leg_left]->set_cone_twist_limit(PI_F / 4.0f, PI_F / 4.0f, 0.0f);

	joints[joint_torso_upper_arm_right] = Physics3DConstraint::cone_twist(world->collision, parts[part_torso], parts[part_upper_arm_right], joints_pos_a[joint_torso_upper_arm_right], joints_pos_b[joint_torso_upper_arm_right], joints_rotate_a[joint_torso_upper_arm_right], joints_rotate_b[joint_torso_upper_arm_right]);
	joints[joint_torso_upper_arm_right]->set_cone_twist_limit(PI_F / 2.0f, PI_F / 2.0f, 0.0f);

	joints[joint_torso_upper_arm_left] = Physics3DConstraint::cone_twist(world->collision, parts[part_torso], parts[part_upper_arm_left], joints_pos_a[joint_torso_upper_arm_left], joints_pos_b[joint_torso_upper_arm_left], joints_rotate_a[joint_torso_upper_arm_left], joints_rotate_b[joint_torso_upper_arm_left]);
	joints[joint_torso_upper_arm_left]->set_cone_twist_limit(PI_F / 2.0f, PI_F / 2.0f, 0.0f);
}

PlayerRagdoll::~PlayerRagdoll()
{
}

SceneObjectPtr PlayerRagdoll::scene_box(const uicore::Vec3f &box_size)
{
	auto model = SceneModel::create(world()->client->scene, create_box(box_size));
	return SceneObject::create(world()->client->scene, model);
}

SceneObjectPtr PlayerRagdoll::scene_capsule(float radius, float height)
{
	auto model = SceneModel::create(world()->client->scene, create_box(Vec3f(radius, height * 0.5f, radius)));
	return SceneObject::create(world()->client->scene, model);
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

void PlayerRagdoll::tick(const GameTick &tick)
{
	for (int i = 0; i < total_parts; i++)
	{
		prev_pos[i] = next_pos[i];
		prev_orientation[i] = next_orientation[i];
		next_pos[i] = parts[i]->position();
		next_orientation[i] = parts[i]->orientation();
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

void PlayerRagdoll::frame(float time_elapsed, float interpolated_time)
{
	for (int i = 0; i < total_parts; i++)
	{
		objects[i]->update(time_elapsed);
		objects[i]->set_position(mix(prev_pos[i], next_pos[i], interpolated_time));
		objects[i]->set_orientation(Quaternionf::slerp(prev_orientation[i], next_orientation[i], interpolated_time));
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
			model_data->meshes[0].tangents.push_back(tangent[i]);
			model_data->meshes[0].bitangents.push_back(bitangent);
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
