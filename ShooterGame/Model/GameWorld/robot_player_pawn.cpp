
#include "precomp.h"
#include "robot_player_pawn.h"
#include "game_master.h"

using namespace uicore;

RobotPlayerPawn::RobotPlayerPawn(GameWorld *world, const std::string &owner, std::shared_ptr<SpawnPoint> spawn) : ServerPlayerPawn(world, owner, spawn)
{
}

void RobotPlayerPawn::apply_impulse(const uicore::Vec3f &force)
{
	ServerPlayerPawn::apply_impulse(force);
	if (mode != RobotPlayerMode::chase)
	{
		mode = RobotPlayerMode::chase;
		weapon_change_cooldown = 15.0f;
	}
}

void RobotPlayerPawn::tick()
{
	ServerPlayerPawn::tick();

	key_forward.next_pressed = false;
	key_back.next_pressed = false;
	key_left.next_pressed = false;
	key_right.next_pressed = false;
	key_jump.next_pressed = false;
	key_fire_primary.next_pressed = false;
	key_fire_secondary.next_pressed = false;
	key_weapon = 0;

	mode_change_cooldown = std::max(mode_change_cooldown - time_elapsed(), 0.0f);
	weapon_change_cooldown = std::max(weapon_change_cooldown - time_elapsed(), 0.0f);

	if (mode_change_cooldown == 0.0f)
	{
		switch ((rand() * 3 + RAND_MAX / 2) / RAND_MAX)
		{
		default:
		case 0: mode = RobotPlayerMode::stationary; break;
		case 1: mode = RobotPlayerMode::chase; break;
		case 2: mode = RobotPlayerMode::path; current_path.clear(); current_path_index = 0; break;
		}
		mode_change_cooldown = 20.0f;
	}

	switch (mode)
	{
	case RobotPlayerMode::stationary: tick_stationary(); break;
	case RobotPlayerMode::chase: tick_chase(); break;
	case RobotPlayerMode::path: tick_path(); break;
	}

	track_target();
}

void RobotPlayerPawn::tick_stationary()
{
}

void RobotPlayerPawn::tick_path()
{
	auto game_master = GameMaster::instance(this);

	if (current_path_index == current_path.size())
	{
		int nearest_segment = -1;
		float nearest_dist2 = 0.0f;

		int index = 0;
		for (const auto &node : game_master->map_data->path_nodes)
		{
			auto v = node.position - get_position();
			float dist2 = Vec3f::dot(v, v);
			if (nearest_segment == -1 || nearest_dist2 > dist2)
			{
				nearest_segment = index;
				nearest_dist2 = dist2;
			}
			index++;
		}

		if (nearest_segment == -1)
			return;

		current_path = create_path_steps(nearest_segment, rand() % game_master->map_data->path_nodes.size(), 100, -1);
		current_path_index = 0;
	}

	if (current_path_index == current_path.size())
		return;

	auto path_point = game_master->map_data->path_nodes[current_path[current_path_index]].position;

	auto path_pos = Vec2f(path_point.x, path_point.z);
	auto robot_pos = Vec2f(get_position().x, get_position().z);
	auto path_dir = Vec2f::normalize(path_pos - robot_pos);

	float wanted_angle = std::acos(path_dir.y) * 180.0f / PI;
	if (path_dir.x < 0.0f)
		wanted_angle = 360.0f - wanted_angle;

	float delta = wanted_angle - dir;
	if (delta < -180.0f)
		delta += 360.0f;
	else if (delta > 180.0f)
		delta -= 360.0f;

	float turn_speed = 120.0f * time_elapsed();

	if (std::abs(delta) < turn_speed)
		dir = wanted_angle;
	else if (delta >= 0.0f)
		dir += turn_speed;
	else
		dir -= turn_speed;

	up = 0.0f;

	float move_fov = 30.0f;
	if (std::abs(dir - wanted_angle) < move_fov)
		key_forward.next_pressed = true;

	auto v = path_pos - robot_pos;
	auto dist2 = Vec2f::dot(v, v);
	if (dist2 < 4.0f)
	{
		//log_event("debug", "Reached path goal %1 (point %2: x = %3, z = %4)", current_path_index, current_path[current_path_index], path_point.x, path_point.z);
		current_path_index++;
		//if (current_path_index == current_path.size())
		//	mode = RobotPlayerMode::idle;
	}
}

std::vector<int> RobotPlayerPawn::create_path_steps(int start_segment, int end_segment, int max_depth, int avoid_segment)
{
	auto game_master = GameMaster::instance(this);

	std::vector<bool> visited(game_master->map_data->path_nodes.size());

	if (avoid_segment != -1)
		visited[avoid_segment] = true;

	std::vector<int> froms;
	std::vector<int> tos;
	std::vector<int> depths;

	int next_step = 0;
	int current_segment = start_segment;
	int current_depth = 0;
	while (current_segment != end_segment && current_depth < max_depth)
	{
		const auto &segment = game_master->map_data->path_nodes[current_segment];

		bool found_end = false;
		for (auto next_segment : segment.connections)
		{
			if (!visited[next_segment])
			{
				visited[next_segment] = true;
				froms.push_back(current_segment);
				tos.push_back(next_segment);
				depths.push_back(current_depth + 1);

				if (next_segment == end_segment)
				{
					found_end = true;
					break;
				}
			}
		}

		if (found_end || next_step == froms.size())
			break;

		current_segment = tos[next_step];
		current_depth = depths[next_step];
		next_step++;
	}

	std::vector<int> path_points;
	path_points.push_back(start_segment);

	if (!froms.empty())
	{
		path_points.resize(depths.back() + 1);

		int current_step = tos.size() - 1;
		while (current_step >= 0)
		{
			auto from_segment = froms[current_step];
			auto to_segment = tos[current_step];

			path_points[depths[current_step]] = to_segment;

			current_step--;
			while (current_step >= 0 && tos[current_step] != from_segment)
			{
				current_step--;
			}
		}
	}

	return path_points;
}

void RobotPlayerPawn::tick_chase()
{
	if (weapon_change_cooldown == 0.0f)
	{
		weapon_change_cooldown = 15.0f;

		float random = rand() / (float)RAND_MAX;
		int weapon_index = (int)std::round(5 * random);
		key_weapon = weapon_index;
	}
}

void RobotPlayerPawn::track_target()
{
	std::shared_ptr<ServerPlayerPawn> target = nullptr;

	for (auto it : GameMaster::instance(this)->server_players)
	{
		auto other_pawn = it.second;
		if (other_pawn.get() != this)
			target = other_pawn;
	}

	if (target)
	{
		aim_angle_cooldown = std::max(aim_angle_cooldown - time_elapsed(), 0.0f);
		if (aim_angle_cooldown == 0.0f)
		{
			float random = rand() / (float)RAND_MAX;
			aim_angle_error = (random - 0.5f) * 40.0f; // To do: difficulty level should affect this
			aim_angle_cooldown = 0.3f;
		}

		auto eye_pos = get_position() + eye_offset;
		auto target_eye_pos = target->get_position() + eye_offset;

		bool line_of_sight = !game_world()->kinematic_collision()->ray_test_any(eye_pos, target_eye_pos, [&](const Physics3DHit &result)
		{
			return result.object->data_object() == nullptr;
		});

		if (line_of_sight)
			last_seen_target_pos = target_eye_pos;

		auto target_pos = Vec2f(last_seen_target_pos.x, last_seen_target_pos.z);
		auto robot_pos = Vec2f(eye_pos.x, eye_pos.z);
		auto target_dir = Vec2f::normalize(target_pos - robot_pos);

		float wanted_angle = std::acos(target_dir.y) * 180.0f / PI;
		if (target_dir.x < 0.0f)
			wanted_angle = 360.0f - wanted_angle;

		wanted_angle += std::fmod(aim_angle_error, 360.0f); 

		if (mode != RobotPlayerMode::path) // To do: this also needs to be active for path, and then path movement shouldn't use view vector
		{
			float delta = wanted_angle - dir;
			if (delta < -180.0f)
				delta += 360.0f;
			else if (delta > 180.0f)
				delta -= 360.0f;

			float turn_speed = 120.0f * time_elapsed();

			if (std::abs(delta) < turn_speed)
				dir = wanted_angle;
			else if (delta >= 0.0f)
				dir += turn_speed;
			else
				dir -= turn_speed;

			up = -std::asin(Vec3f::normalize(last_seen_target_pos - eye_pos).y) * 180.0f / PI;
		}

		float shoot_fov = 30.0f;
		if (std::abs(dir - wanted_angle) < shoot_fov && line_of_sight)
		{
			key_fire_primary.next_pressed = true;
			if (mode == RobotPlayerMode::chase)
				key_forward.next_pressed = true;
		}
	}
}
