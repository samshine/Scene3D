
#include "precomp.h"
#include "player_ragdoll.h"
#include "game_world.h"
#include "game_object_collision.h"
#include "player_pawn.h"
#include "Model/game.h"
#include <algorithm>

using namespace uicore;

PlayerRagdoll::PlayerRagdoll(GameWorld *world, const Vec3f &pos, const Quaternionf &orientation) : GameObject(world)
{
	Vec3f box_size(1.0f, 1.0f, 2.72f);

	//SceneModel model(world->game()->gc(), world->game()->scene, "Thalania/Thalania.cmodel");
	auto model = SceneModel::create(world->game()->scene, create_box(box_size));

	//scene_object1.set_scale(Vec3f(0.15f));
	//scene_object1.play_animation("static");

	scene_object1 = SceneObject::create(world->game()->scene, model);
	scene_object1->set_position(pos);
	scene_object1->set_orientation(orientation);

	scene_object2 = SceneObject::create(world->game()->scene, model);
	scene_object2->set_position(pos);
	scene_object2->set_orientation(orientation);

	auto shape = Physics3DShape::box(box_size);
	physics_object1 = Physics3DObject::rigid_body(world->game()->collision, shape, 1.0f, pos, orientation);
	physics_object1->set_ccd_swept_sphere_radius(1.0f);
	physics_object1->set_ccd_motion_threshold(0.5f);

	physics_object2 = Physics3DObject::rigid_body(world->game()->collision, shape, 1.0f, pos + orientation.rotate_vector(Vec3f(0.0f, 0.0f, 1.0f)), Quaternionf::inverse(orientation));
	physics_object2->set_ccd_swept_sphere_radius(1.0f);
	physics_object2->set_ccd_motion_threshold(0.5f);

	hinge_constraint = Physics3DConstraint::hinge(world->game()->collision, physics_object1, physics_object2, Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f), Quaternionf(90.0f, 0.0f, 0.0f, angle_degrees, order_YXZ), Quaternionf(90.0f, 0.0f, 0.0f, angle_degrees, order_YXZ));

	physics_object1->apply_central_impulse(orientation.rotate_vector(Vec3f(0.0f, 0.0f, 50.0f)));

	// http://caiosabino.com/?p=26


	/*
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

	for (int i = 0; i < total_parts; i++)
		parts[i] = Physics3DObject::rigid_body(world->game()->collision, shapes[i], 1.0f);

	Vec3f pos_a;
	Vec3f pos_b;
	Quaternionf rotate_a;
	Quaternionf rotate_b;

	joints[joint_head] = Physics3DConstraint::cone_twist(world->game()->collision, parts[part_torso], parts[part_head], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_head].set_cone_twist_limit(0.7f, 0.5f, 0.0f);

	joints[joint_upper_arm_right_lower_arm_right] = Physics3DConstraint::hinge(world->game()->collision, parts[part_upper_arm_right], parts[part_lower_arm_right], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_upper_arm_right_lower_arm_right].set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_arm_left_lower_arm_left] = Physics3DConstraint::hinge(world->game()->collision, parts[part_upper_arm_left], parts[part_lower_arm_left], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_upper_arm_left_lower_arm_left].set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_leg_right_lower_leg_right] = Physics3DConstraint::hinge(world->game()->collision, parts[part_upper_leg_right], parts[part_lower_leg_right], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_upper_leg_right_lower_leg_right].set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_upper_leg_left_lower_leg_left] = Physics3DConstraint::hinge(world->game()->collision, parts[part_upper_leg_left], parts[part_lower_leg_left], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_upper_leg_left_lower_leg_left].set_hinge_limit(-PI_F / 2.0f, 0);

	joints[joint_torso_upper_leg_right] = Physics3DConstraint::cone_twist(world->game()->collision, parts[part_torso], parts[part_upper_leg_right], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_torso_upper_leg_right].set_cone_twist_limit(PI_F / 4.0f, PI_F / 4.0f);

	joints[joint_torso_upper_leg_left] = Physics3DConstraint::cone_twist(world->game()->collision, parts[part_torso], parts[part_upper_leg_left], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_torso_upper_leg_left].set_cone_twist_limit(PI_F / 4.0f, PI_F / 4.0f);

	joints[joint_torso_upper_arm_right] = Physics3DConstraint::cone_twist(world->game()->collision, parts[part_torso], parts[part_upper_arm_right], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_torso_upper_arm_right].set_cone_twist_limit(PI_F / 2.0f, PI_F / 2.0f);

	joints[joint_torso_upper_arm_left] = Physics3DConstraint::cone_twist(world->game()->collision, parts[part_torso], parts[part_upper_arm_left], pos_a, pos_b, rotate_a, rotate_b);
	joints[joint_torso_upper_arm_left].set_cone_twist_limit(PI_F / 2.0f, PI_F / 2.0f);
	*/
}

PlayerRagdoll::~PlayerRagdoll()
{
}

void PlayerRagdoll::tick(const GameTick &tick)
{
	prev_pos1 = next_pos1;
	prev_orientation1 = next_orientation1;

	prev_pos2 = next_pos2;
	prev_orientation2 = next_orientation2;

	next_pos1 = physics_object1->position();
	next_orientation1 = physics_object1->orientation();

	next_pos2 = physics_object2->position();
	next_orientation2 = physics_object2->orientation();

	if (first_tick)
	{
		prev_pos1 = next_pos1;
		prev_orientation1 = next_orientation1;

		prev_pos2 = next_pos2;
		prev_orientation2 = next_orientation2;

		first_tick = false;
	}
}

void PlayerRagdoll::frame(float time_elapsed, float interpolated_time)
{
	scene_object1->update(time_elapsed);
	scene_object1->set_position(mix(prev_pos1, next_pos1, interpolated_time));
	scene_object1->set_orientation(Quaternionf::slerp(prev_orientation1, next_orientation1, interpolated_time));

	scene_object2->update(time_elapsed);
	scene_object2->set_position(mix(prev_pos2, next_pos2, interpolated_time));
	scene_object2->set_orientation(Quaternionf::slerp(prev_orientation2, next_orientation2, interpolated_time));
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
