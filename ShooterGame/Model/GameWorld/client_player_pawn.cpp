
#include "precomp.h"
#include "client_player_pawn.h"
#include "collision_game_object.h"
#include "game_master.h"
#include "Model/Settings/settings.h"
#include <algorithm>

using namespace uicore;

ClientPlayerPawn::ClientPlayerPawn(GameWorld *world) : PlayerPawn(world)
{
	camera = SceneCamera::create(game_world()->client()->scene());
	camera_shape = Physics3DShape::sphere(0.5f);

	func_received_event("create") = [this](const std::string &sender, const JsonValue &net_event) { net_create(net_event); };
	func_received_event("player-pawn-update") = [this](const std::string &sender, const JsonValue &net_event) { net_update(net_event); };
	func_received_event("player-pawn-hit") = [this](const std::string &sender, const JsonValue &net_event) { net_hit(net_event); };
}

ClientPlayerPawn::~ClientPlayerPawn()
{
}

void ClientPlayerPawn::net_create(const JsonValue &net_event)
{
	net_update(net_event);

	if (is_owner)
	{
		game_world()->client()->scene_camera() = camera;
		GameMaster::instance()->client_player = cast<ClientPlayerPawn>();
	}
}

void ClientPlayerPawn::net_update(const JsonValue &net_event)
{
	is_owner = net_event["owner"].to_boolean();
	health = net_event["health"].to_float();

	if (is_owner)
	{
		// Remove obsolete entries
		while (!sent_movements.empty() && sent_movements.front().tick_time < tick_time())
			sent_movements.erase(sent_movements.begin());

		// Do nothing if this packet is older than anything we have in our move buffer
		if (sent_movements.empty() || sent_movements.front().tick_time != tick_time())
		{
			return;
		}

		// Erase movement from the past
		sent_movements.erase(sent_movements.begin());

		// Grab server position and velocity received by server
		Vec3f pos, velocity;
		pos.x = net_event["x"].to_float();
		pos.y = net_event["y"].to_float();
		pos.z = net_event["z"].to_float();
		velocity.x = net_event["vx"].to_float();
		velocity.y = net_event["vy"].to_float();
		velocity.z = net_event["vz"].to_float();
		bool is_flying = net_event["flying"].to_boolean();
		character_controller.warp(pos, velocity, is_flying);

		// Replay movements until now to get new predicted client position
		for (const auto &movement : sent_movements)
			update_character_controller(movement);
	}
	else
	{
		key_forward.next_pressed = net_event["forward"].to_boolean();
		key_back.next_pressed = net_event["back"].to_boolean();
		key_left.next_pressed = net_event["left"].to_boolean();
		key_right.next_pressed = net_event["right"].to_boolean();
		key_jump.next_pressed = net_event["jump"].to_boolean();
		key_fire_primary.next_pressed = net_event["firePrimary"].to_boolean();
		key_fire_secondary.next_pressed = net_event["fireSecondary"].to_boolean();
		key_weapon = net_event["weapon"].to_int();
		dir = net_event["dir"].to_float();
		up = net_event["up"].to_float();

		Vec3f pos, velocity;
		pos.x = net_event["x"].to_float();
		pos.y = net_event["y"].to_float();
		pos.z = net_event["z"].to_float();
		velocity.x = net_event["vx"].to_float();
		velocity.y = net_event["vy"].to_float();
		velocity.z = net_event["vz"].to_float();
		bool is_flying = net_event["flying"].to_boolean();
		character_controller.warp(pos, velocity, is_flying);
	}
}

void ClientPlayerPawn::net_hit(const JsonValue &net_event)
{
	if (!sound || !sound->playing())
	{
		sound = AudioObject::create(game_world()->client()->audio());
		switch (rand() * 4 / (RAND_MAX + 1))
		{
		default:
		case 0: sound->set_sound("Sound/Character/hurt1.ogg"); break;
		case 1: sound->set_sound("Sound/Character/hurt2.ogg"); break;
		case 2: sound->set_sound("Sound/Character/hurt3.ogg"); break;
		case 3: sound->set_sound("Sound/Character/hurt4.ogg"); break;
		}
		sound->set_attenuation_begin(1.0f);
		sound->set_attenuation_end(20.0f);
		sound->set_volume(0.5f);
		sound->set_position(get_position());
		sound->play();
	}
}

void ClientPlayerPawn::tick()
{
	if (is_owner)
	{
		key_forward.next_pressed = game_world()->client()->button_down("move-forward");
		key_back.next_pressed = game_world()->client()->button_down("move-back");
		key_left.next_pressed = game_world()->client()->button_down("move-left");
		key_right.next_pressed = game_world()->client()->button_down("move-right");
		key_jump.next_pressed = game_world()->client()->button_down("jump");
		key_fire_primary.next_pressed = game_world()->client()->button_down("fire-primary");
		key_fire_secondary.next_pressed = game_world()->client()->button_down("fire-secondary");
		key_weapon = 0;
		if (game_world()->client()->button_down("switch-to-icelauncher")) key_weapon = 1;
		else if (game_world()->client()->button_down("switch-to-rocketlauncher")) key_weapon = 2;
		else if (game_world()->client()->button_down("switch-to-dualguns")) key_weapon = 3;
		else if (game_world()->client()->button_down("switch-to-sniperrifle")) key_weapon = 4;
		else if (game_world()->client()->button_down("switch-to-tractorbeam")) key_weapon = 5;

		auto message = JsonValue::object();
		message["name"] = JsonValue::string("player-pawn-input");
		message["forward"] = JsonValue::boolean(key_forward.next_pressed);
		message["back"] = JsonValue::boolean(key_back.next_pressed);
		message["left"] = JsonValue::boolean(key_left.next_pressed);
		message["right"] = JsonValue::boolean(key_right.next_pressed);
		message["jump"] = JsonValue::boolean(key_jump.next_pressed);
		message["firePrimary"] = JsonValue::boolean(key_fire_primary.next_pressed);
		message["fireSecondary"] = JsonValue::boolean(key_fire_secondary.next_pressed);
		message["weapon"] = JsonValue::number(key_weapon);
		message["dir"] = JsonValue::number(dir);
		message["up"] = JsonValue::number(up);
		send_event("server", message);
	}

	bool was_moving = animation_move_speed > 0.0f;

	PlayerPawn::tick();

	if (is_owner)
	{
		sent_movements.push_back(last_movement);
	}

	/*
	static FilePtr file = File::create_always("c:\\development\\debug_client_player.csv");
	auto str = string_format("%1;%2;%3;%4;%5;", receive_tick_time(), arrival_tick_time(), character_controller.get_position().x, character_controller.get_position().y, character_controller.get_position().z);
	str += string_format("%1;%2;%3;%4\r\n", character_controller.get_velocity().x, character_controller.get_velocity().y, character_controller.get_velocity().z, character_controller.is_flying());
	file->write(str.data(), str.length());
	*/

	bool is_moving = animation_move_speed > 0.0f;

	if (character_controller.get_land_impact() > 3.0f)
	{
		step_movement = 0.0f;

		auto sound_land = AudioObject::create(game_world()->client()->audio());

		shake_camera(0.04f, 0.2f);

		if (character_controller.get_land_impact() > 17.0f)
		{
			sound_land->set_volume(0.5f);
			switch (rand() * 4 / (RAND_MAX + 1))
			{
			default:
			case 0: sound_land->set_sound("Sound/Character/hurt1.ogg"); break;
			case 1: sound_land->set_sound("Sound/Character/hurt2.ogg"); break;
			case 2: sound_land->set_sound("Sound/Character/hurt3.ogg"); break;
			case 3: sound_land->set_sound("Sound/Character/hurt4.ogg"); break;
			}
		}
		else
		{
			sound_land->set_volume(0.25f);
			sound_land->set_sound("Sound/Character/land.ogg");
		}

		if (!is_owner)
		{
			sound_land->set_attenuation_begin(1.0f);
			sound_land->set_attenuation_end(10.0f);
			sound_land->set_position(character_controller.get_position());
		}

		sound_land->play();
	}
	else if ((is_moving && !character_controller.is_flying()))
	{
		step_movement += animation_move_speed;
		float step_distance = 2.8f;
		if (step_movement > step_distance)
		{
			step_movement = std::fmod(step_movement, step_distance);
			left_step = !left_step;

			auto sound_footstep = AudioObject::create(game_world()->client()->audio());
			if (left_step)
			{
				if (rand() > RAND_MAX / 2)
					sound_footstep->set_sound("Sound/Character/foot_left1.ogg");
				else
					sound_footstep->set_sound("Sound/Character/foot_left2.ogg");
			}
			else
			{
				if (rand() > RAND_MAX / 2)
					sound_footstep->set_sound("Sound/Character/foot_right1.ogg");
				else
					sound_footstep->set_sound("Sound/Character/foot_right2.ogg");
			}
			sound_footstep->set_volume(0.05f);

			if (!is_owner)
			{
				sound_footstep->set_attenuation_begin(1.0f);
				sound_footstep->set_attenuation_end(10.0f);
				sound_footstep->set_position(character_controller.get_position());
			}

			sound_footstep->play();
		}
	}

	if (anim != last_anim)
	{
		last_anim = anim;

		if (anim == "jump" || anim == "dodge-left" || anim == "dodge-right" || anim == "dodge-forward" || anim == "dodge-backward")
			shake_camera(0.04f, 0.2f);

		if (anim == "jump")
		{
			auto sound = AudioObject::create(game_world()->client()->audio());
			if (rand() > RAND_MAX / 2)
				sound->set_sound("Sound/Character/jump1.ogg");
			else
				sound->set_sound("Sound/Character/jump2.ogg");
			sound->set_volume(0.5f);

			if (!is_owner)
			{
				sound->set_attenuation_begin(1.0f);
				sound->set_attenuation_end(10.0f);
				sound->set_position(character_controller.get_position());
			}

			sound->play();
		}

		if (scene_object)
			scene_object->play_animation(anim, false);
	}


	if (last_position != Vec3f() || next_position != Vec3f())
	{
		last_position = next_position;
		next_position = character_controller.get_position();
	}
	else
	{
		next_position = character_controller.get_position();
		last_position = next_position;
	}
}

void ClientPlayerPawn::frame()
{
	float mouse_speed_multiplier = 1.0f;

	dir = std::remainder(dir + game_world()->client()->mouse_movement().x * Settings::mouse_speed_x() * mouse_speed_multiplier * 0.01f, 360.0f);
	up = clamp(up + game_world()->client()->mouse_movement().y * Settings::mouse_speed_y() * mouse_speed_multiplier * 0.01f, -90.0f, 90.0f);

	bool first_person_camera = true;

	Quaternionf look_dir = Quaternionf(up, dir, 0.0f, angle_degrees, order_YXZ);
	Vec3f look_pos = mix(last_position, next_position, frame_interpolated_time()) + eye_offset;

	/*
	static FilePtr file = File::create_always("c:\\development\\debug_frame_pos.csv");
	static float total_elapsed = 0.0f;
	total_elapsed += time_elapsed;
	auto str = string_format("%1;%2;%3;%4;%5;", total_elapsed, interpolated_time, last_position.x, last_position.y, last_position.z);
	str += string_format("%1;%2;%3;%4;%5;%6\r\n", next_position.x, next_position.y, next_position.z, look_pos.x, look_pos.y, look_pos.z);
	file->write(str.data(), str.length());
	*/

	shake_timer = std::max(shake_timer - frame_time_elapsed() * shake_speed, 0.0f);
	if (shake_timer > 0.0f)
		look_pos.y += std::sin(shake_timer * 2.0f * PI) * shake_magnitude;

	float available_zoom_out = 0.0f;

	if (!first_person_camera)
	{
		float max_zoom_out = 15.0f;
		Vec3f ideal_look_pos = look_pos + look_dir.rotate_vector(Vec3f(0.0f, 0.0f, -max_zoom_out));

		auto hit = game_world()->kinematic_collision()->sweep_test_nearest(camera_shape, look_pos, Quaternionf(), ideal_look_pos, Quaternionf(), 0.0f, [&](const Physics3DHit &result)
		{
			return result.object->data<CollisionGameObject>() == nullptr; // Ignore all game objects
		});

		if (hit)
		{
			available_zoom_out = look_pos.distance(hit.position);
		}
		else
		{
			available_zoom_out = max_zoom_out;
		}
	}
	else
	{
		available_zoom_out = 0.0f;
	}

	zoom_out = std::min(zoom_out + frame_time_elapsed() * 5.0f, available_zoom_out);

	bool show_mesh = false;

	if (zoom_out > 3.0f)
	{
		look_pos = look_pos + look_dir.rotate_vector(Vec3f(0.0f, 0.0f, -zoom_out));
		show_mesh = true;
	}

	if (!is_owner)
		show_mesh = true;

	if (show_mesh)
	{
		if (!scene_object)
		{
			auto model = SceneModel::create(game_world()->client()->scene(), "Models/Kachujin/Kachujin.cmodel");
			scene_object = SceneObject::create(game_world()->client()->scene(), model);

			if (animation_move_speed > 0.0f)
				scene_object->play_animation("forward", false);
			else
				scene_object->play_animation("default", false);
		}

		scene_object->set_position(mix(last_position, next_position, frame_interpolated_time()));
		scene_object->set_orientation(Quaternionf(clamp(-up * 0.5f, -15.0f, 15.0f), 180.0f + dir, 0.0f, angle_degrees, order_YXZ));
		scene_object->update(frame_time_elapsed());
	}
	else
	{
		scene_object = nullptr;
	}

	camera->set_position(look_pos);
	camera->set_orientation(look_dir);

	weapon->frame(frame_time_elapsed(), frame_interpolated_time());
}

void ClientPlayerPawn::shake_camera(float magnitude, float duration)
{
	if (shake_timer == 0.0f)
	{
		shake_magnitude = magnitude;
		shake_timer = 1.0f;
		shake_speed = 1.0f / duration;
	}
}
