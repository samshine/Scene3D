
#include "precomp.h"
#include "elevator.h"
#include "collision_game_object.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

Elevator::Elevator(GameWorld *world, const Vec3f &pos1, const Vec3f &pos2, const Quaternionf &orientation, const std::string &model_name, float scale)
: GameObject(world), pos1(pos1), pos2(pos2), orientation(orientation)
{
	box_size.x *= scale;
	box_size.z *= scale;
	box_shape = Physics3DShape::box(box_size);
	body = Physics3DObject::collision_body(game_world()->kinematic_collision(), box_shape, pos1, orientation);
	body->set_kinematic_object();

	if (game_world()->client())
	{
		auto model = SceneModel::create(game_world()->client()->scene(), model_name);
		scene_object = SceneObject::create(game_world()->client()->scene(), model, pos1, orientation, Vec3f(scale));

		// For debugging collision box
		//auto model = SceneModel::create(game_world()->client()->scene(), create_box());
		//scene_object = SceneObject::create(game_world()->client()->scene(), model, pos1, orientation, Vec3f(1.0f));

		func_received_event("elevator-update") = bind_member(this, &Elevator::on_elevator_update);
	}
}

Elevator::~Elevator()
{
}

void Elevator::on_elevator_update(const std::string &sender, const uicore::JsonValue &message)
{
	state = (State)message["state"].to_int();
	time = message["time"].to_float();
}

void Elevator::send_net_update(const std::string &target)
{
	JsonValue message = JsonValue::object();
	message["name"] = JsonValue::string("elevator-update");
	message["state"] = JsonValue::number((int)state);
	message["time"] = JsonValue::number(time);
	send_event(target, message);
}

void Elevator::tick()
{
	switch (state)
	{
	case state_down: tick_down(); break;
	case state_start_triggered: tick_start_triggered(); break;
	case state_moving_up: tick_moving_up(); break;
	case state_up: tick_up(); break;
	case state_moving_down: tick_moving_down(); break;
	}
}

void Elevator::tick_down()
{
	if (test_start_trigger())
	{
		state = state_start_triggered;
		time = 0.250f;

		if (!game_world()->client())
			send_net_update("all");
	}
}

void Elevator::tick_start_triggered()
{
	time = std::max(time - time_elapsed(), 0.0f);
	if (time == 0.0f)
	{
		state = state_moving_up;

		if (scene_object)
			scene_object->play_animation("up", true);

		if (!game_world()->client())
			send_net_update("all");
	}
}

void Elevator::tick_moving_up()
{
	float new_time = std::min(time + time_elapsed() * speed, 1.0f);

	if (scene_object)
	{
		float distance = std::abs((new_time - time) * pos1.distance(pos2));
		scene_object->moved(distance);
	}

	Vec3f from_pos = mix(pos1, pos2, time);
	Vec3f to_pos = mix(pos1, pos2, new_time);

	for (const auto &hit : game_world()->kinematic_collision()->sweep_test_all(box_shape, from_pos, orientation, to_pos, orientation))
	{
		PlayerPawn *hit_player = hit.object->data<PlayerPawn>();
		if (hit_player)
		{
			hit_player->ground_moved(to_pos - from_pos);
		}
	}

	time = new_time;

	Vec3f obj_pos = mix(pos1, pos2, time);
	body->set_position(obj_pos);
	if (scene_object)
		scene_object->set_position(obj_pos);

	if (time == 1.0f)
	{
		state = state_up;
		time = wait_time;

		if (scene_object)
			scene_object->play_animation("default", true);

		if (!game_world()->client())
			send_net_update("all");
	}
}

void Elevator::tick_up()
{
	time = std::max(time - time_elapsed(), 0.0f);

	if (time == 0.0f)
	{
		state = state_moving_down;
		time = 1.0f;

		if (scene_object)
			scene_object->play_animation("down", true);

		if (!game_world()->client())
			send_net_update("all");
	}
}

void Elevator::tick_moving_down()
{
	float new_time = std::max(time - time_elapsed() * speed, 0.0f);

	if (scene_object)
	{
		float distance = std::abs((new_time - time) * pos1.distance(pos2));
		scene_object->moved(distance);
	}

	time = new_time;

	Vec3f obj_pos = mix(pos1, pos2, time);
	body->set_position(obj_pos);
	if (scene_object)
		scene_object->set_position(obj_pos);

	if (time == 0.0f)
	{
		state = state_down;

		if (scene_object)
			scene_object->play_animation("default", true);

		if (!game_world()->client())
			send_net_update("all");
	}
}

bool Elevator::test_start_trigger()
{
	Vec3f trigger_pos = pos1 + Vec3f(0.0, 1.0f, 0.0f);

	return game_world()->kinematic_collision()->sweep_test_any(box_shape, pos1, orientation, trigger_pos, orientation, 0.0f, [](const Physics3DHit &result)
	{
		return result.object->data<PlayerPawn>() != nullptr;
	});
}

std::shared_ptr<ModelData> Elevator::create_box()
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
