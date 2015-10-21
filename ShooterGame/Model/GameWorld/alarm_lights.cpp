
#include "precomp.h"
#include "alarm_lights.h"
#include "game_world.h"

using namespace uicore;

AlarmLights::AlarmLights(GameWorld *world) : GameObject(world)
{
	for (const auto &item : world->level_data["objects"].items())
	{
		if (item["type"].to_string() != "alarmLights") continue;

		std::string group = item["fields"]["group"].to_string();

		auto model = SceneModel::create(world->client->scene, item["mesh"].to_string());
		auto light = SceneObject::create(world->client->scene, model);
		light->set_scale(Vec3f(item["scale"].to_float()));
		light->set_position(Vec3f(item["position"]["x"].to_float(), item["position"]["y"].to_float(), item["position"]["z"].to_float()));
		light->set_orientation(Quaternionf(item["up"].to_float(), item["dir"].to_float(), item["tilt"].to_float(), angle_degrees, order_YXZ));
		light->play_animation("blinking", true);

		groups[group].push_back(light);
	}
}

AlarmLights::~AlarmLights()
{
}

void AlarmLights::tick(const GameTick &tick)
{
}

void AlarmLights::frame(float time_elapsed, float interpolated_time)
{
	for (auto it : groups)
	{
		for (SceneObjectPtr &obj : it.second)
		{
			obj->update(time_elapsed);
		}
	}
}

void AlarmLights::play_group_animation(const std::string &group_id, const std::string &animation_name)
{
	auto it = groups.find(group_id);
	if (it != groups.end())
	{
		for (SceneObjectPtr &obj : it->second)
		{
			obj->play_animation(animation_name, true);
		}
	}
}
