
#include "precomp.h"
#include "alarm_lights.h"
#include "game_world.h"

using namespace uicore;

AlarmLights::AlarmLights(GameWorld *world) : GameObject(world)
{
	for (const auto &item : world->map_data->objects)
	{
		if (item.type != "AlarmLight") continue;

		std::string group = item.fields["group"].to_string();

		auto model = SceneModel::create(world->client->scene, item.mesh);
		auto light = SceneObject::create(world->client->scene, model);
		light->set_scale(Vec3f(item.scale));
		light->set_position(item.position);
		light->set_orientation(Quaternionf(item.up, item.dir, item.tilt, angle_degrees, order_YXZ));
		light->play_animation(item.animation, true);

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
