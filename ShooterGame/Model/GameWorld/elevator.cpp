
#include "precomp.h"
#include "elevator.h"
#include "game_world.h"
#include "game_tick.h"
#include "collision_game_object.h"
#include "player_pawn.h"
#include <algorithm>

using namespace uicore;

Elevator::Elevator(GameWorld *world, int level_obj_id, const Vec3f &pos1, const Vec3f &pos2, const Quaternionf &orientation, const std::string &model_name, float scale)
: GameObject(world), level_obj_id(level_obj_id), pos1(pos1), pos2(pos2), orientation(orientation)
{
	box_shape = Physics3DShape::box(box_size);
	body = Physics3DObject::collision_body(world->collision, box_shape, pos1, orientation);
	body->set_kinematic_object();

	if (world->client)
	{
		auto model = SceneModel::create(world->client->scene, model_name);
		scene_object = SceneObject::create(world->client->scene, model, pos1, orientation, Vec3f(scale));

		// For debugging collision box
		//SceneModel model(world->client->window->gc(), *world->client->scene, create_box());
		//scene_object = SceneObject(*world->client->scene, model, pos1, orientation, Vec3f(1.0f));
	}
}

Elevator::~Elevator()
{
}

void Elevator::net_update(const GameTick &net_tick, const uicore::NetGameEvent &net_event)
{
	if (world()->client)
	{
		int age = std::max(net_tick.arrival_tick_time - net_tick.receive_tick_time, 0);

		state = (State)net_event.get_argument(1).get_integer();
		time = net_event.get_argument(2).get_number();

		for (int i = 0; i < age; i++)
		{
			GameTick past_tick = net_tick;
			past_tick.receive_tick_time += i;
			past_tick.arrival_tick_time += i;
			tick(past_tick);
		}
	}
}

void Elevator::send_net_update(const GameTick &tick, const std::string &target)
{
	NetGameEvent net_event("elevator-update");

	net_event.add_argument(level_obj_id);
	net_event.add_argument((int)state);
	net_event.add_argument(time);

	world()->network->queue_event(target, net_event, tick.arrival_tick_time);
}

void Elevator::tick(const GameTick &tick)
{
	switch (state)
	{
	case state_down: tick_down(tick); break;
	case state_start_triggered: tick_start_triggered(tick); break;
	case state_moving_up: tick_moving_up(tick); break;
	case state_up: tick_up(tick); break;
	case state_moving_down: tick_moving_down(tick); break;
	}
}

void Elevator::tick_down(const GameTick &tick)
{
	if (test_start_trigger())
	{
		state = state_start_triggered;
		time = 0.250f;

		if (!world()->client)
			send_net_update(tick, "all");
	}
}

void Elevator::tick_start_triggered(const GameTick &tick)
{
	time = std::max(time - tick.time_elapsed, 0.0f);
	if (time == 0.0f)
	{
		state = state_moving_up;

		if (scene_object)
			scene_object->play_animation("up", true);

		if (!world()->client)
			send_net_update(tick, "all");
	}
}

void Elevator::tick_moving_up(const GameTick &tick)
{
	float new_time = std::min(time + tick.time_elapsed * speed, 1.0f);

	if (scene_object)
	{
		float distance = std::abs((new_time - time) * pos1.distance(pos2));
		scene_object->moved(distance);
	}

	Vec3f from_pos = mix(pos1, pos2, time);
	Vec3f to_pos = mix(pos1, pos2, new_time);

	auto test = Physics3DSweepTest::create(world()->collision);
	test->test_all_hits(box_shape, from_pos, orientation, to_pos, orientation);
	for (int i = 0; i < test->hit_count(); i++)
	{
		Physics3DObjectPtr obj = test->hit_object(i);
		PlayerPawn *hit_player = obj->data<PlayerPawn>();
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

		if (!world()->client)
			send_net_update(tick, "all");
	}
}

void Elevator::tick_up(const GameTick &tick)
{
	time = std::max(time - tick.time_elapsed, 0.0f);

	if (time == 0.0f)
	{
		state = state_moving_down;
		time = 1.0f;

		if (scene_object)
			scene_object->play_animation("down", true);

		if (!world()->client)
			send_net_update(tick, "all");
	}
}

void Elevator::tick_moving_down(const GameTick &tick)
{
	float new_time = std::max(time - tick.time_elapsed * speed, 0.0f);

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

		if (!world()->client)
			send_net_update(tick, "all");
	}
}

bool Elevator::test_start_trigger()
{
	Vec3f trigger_pos = pos1 + Vec3f(0.0, 1.0f, 0.0f);

	auto test = Physics3DSweepTest::create(world()->collision);
	test->test_all_hits(box_shape, pos1, orientation, trigger_pos, orientation);

	for (int i = 0; i < test->hit_count(); i++)
	{
		Physics3DObjectPtr obj = test->hit_object(i);
		PlayerPawn *hit_player = obj->data<PlayerPawn>();
		if (hit_player)
		{
			return true;
		}
	}

	return false;
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
