
#include "precomp.h"
#include "client_player_pawn.h"
#include "game_tick.h"
#include "game_object_collision.h"
#include "Model/game.h"
#include <algorithm>

using namespace clan;

ClientPlayerPawn::ClientPlayerPawn(GameWorld *world) : PlayerPawn(world)
{
	camera = SceneCamera(world->game()->scene);
	camera_shape = Physics3DShape::sphere(0.5f);
	camera_sweep_test = Physics3DSweepTest(world->game()->collision);
}

ClientPlayerPawn::~ClientPlayerPawn()
{

}

void ClientPlayerPawn::net_create(const GameTick &tick, const clan::NetGameEvent &net_event)
{
	net_update(tick, net_event);

	if (is_owner)
		world()->game()->scene.set_camera(camera);
}

void ClientPlayerPawn::net_update(const GameTick &tick, const clan::NetGameEvent &net_event)
{
	is_owner = net_event.get_argument(1).get_boolean();
	health = net_event.get_argument(19).get_number();

	if (is_owner)
	{
		// Save current view dir
		PlayerPawnMovement present_movement = cur_movement;

		// Remove obsolete entries
		while (!sent_movements.empty() && sent_movements.front().tick_time < tick.receive_tick_time)
			sent_movements.erase(sent_movements.begin());

		// Do nothing if this packet is older than anything we have in our move buffer
		if (sent_movements.empty() || sent_movements.front().tick_time != tick.receive_tick_time)
		{
			return;
		}

		// Grab movement from the past
		cur_movement = sent_movements.front();
		sent_movements.erase(sent_movements.begin());

		// Grab input from server
		cur_movement.key_forward.next_pressed = net_event.get_argument(2).get_boolean();
		cur_movement.key_back.next_pressed = net_event.get_argument(3).get_boolean();
		cur_movement.key_left.next_pressed = net_event.get_argument(4).get_boolean();
		cur_movement.key_right.next_pressed = net_event.get_argument(5).get_boolean();
		cur_movement.key_jump.next_pressed = net_event.get_argument(6).get_boolean();
		cur_movement.key_fire_primary.next_pressed = net_event.get_argument(7).get_boolean();
		cur_movement.key_fire_secondary.next_pressed = net_event.get_argument(8).get_boolean();
		cur_movement.key_weapon = net_event.get_argument(9).get_integer();
		cur_movement.dir = net_event.get_argument(10).get_number();
		cur_movement.up = net_event.get_argument(11).get_number();

		// Grab server position and velocity received by server
		Vec3f pos, velocity;
		pos.x = net_event.get_argument(12).get_number();
		pos.y = net_event.get_argument(13).get_number();
		pos.z = net_event.get_argument(14).get_number();
		velocity.x = net_event.get_argument(15).get_number();
		velocity.y = net_event.get_argument(16).get_number();
		velocity.z = net_event.get_argument(17).get_number();
		bool is_flying = net_event.get_argument(18).get_boolean();
		character_controller.warp(pos, velocity, is_flying);

		// Replay movement
		update_character_controller(tick.time_elapsed);

		//Console::write_line("Received dir %1 in tick %2", cur_movement.dir, tick.receive_tick_time);

		// Replay movements until now to get new predicted client position
		for (auto &movement : sent_movements)
		{
			cur_movement.key_forward.next_pressed = movement.key_forward.next_pressed;
			cur_movement.key_back.next_pressed = movement.key_back.next_pressed;
			cur_movement.key_left.next_pressed = movement.key_left.next_pressed;
			cur_movement.key_right.next_pressed = movement.key_right.next_pressed;
			cur_movement.key_jump.next_pressed = movement.key_jump.next_pressed;
			cur_movement.key_fire_primary.next_pressed = movement.key_fire_primary.next_pressed;
			cur_movement.key_fire_secondary.next_pressed = movement.key_fire_secondary.next_pressed;
			cur_movement.key_weapon = movement.key_weapon;
			cur_movement.dir = movement.dir;
			cur_movement.up = movement.up;
			int tick_time = movement.tick_time;
			movement = cur_movement;
			movement.tick_time = tick_time;

			update_character_controller(tick.time_elapsed);
		}

		// Restore current view dir
		cur_movement.dir = present_movement.dir;
		cur_movement.up = present_movement.up;
	}
	else
	{
		cur_movement.tick_time = tick.receive_tick_time;
		cur_movement.key_forward.next_pressed = net_event.get_argument(2).get_boolean();
		cur_movement.key_back.next_pressed = net_event.get_argument(3).get_boolean();
		cur_movement.key_left.next_pressed = net_event.get_argument(4).get_boolean();
		cur_movement.key_right.next_pressed = net_event.get_argument(5).get_boolean();
		cur_movement.key_jump.next_pressed = net_event.get_argument(6).get_boolean();
		cur_movement.key_fire_primary.next_pressed = net_event.get_argument(7).get_boolean();
		cur_movement.key_fire_secondary.next_pressed = net_event.get_argument(8).get_boolean();
		cur_movement.key_weapon = net_event.get_argument(9).get_integer();
		cur_movement.dir = net_event.get_argument(10).get_number();
		cur_movement.up = net_event.get_argument(11).get_number();

		Vec3f pos, velocity;
		pos.x = net_event.get_argument(12).get_number();
		pos.y = net_event.get_argument(13).get_number();
		pos.z = net_event.get_argument(14).get_number();
		velocity.x = net_event.get_argument(15).get_number();
		velocity.y = net_event.get_argument(16).get_number();
		velocity.z = net_event.get_argument(17).get_number();
		bool is_flying = net_event.get_argument(18).get_boolean();
		character_controller.warp(pos, velocity, is_flying);
	}
}

void ClientPlayerPawn::net_hit(const GameTick &tick, const clan::NetGameEvent &net_event)
{
	sound = AudioObject(*world()->game()->audio.get());
	sound.set_sound("Sound/shit01.wav");
	sound.set_attenuation_begin(1.0f);
	sound.set_attenuation_end(100.0f);
	sound.set_volume(0.5f);
	sound.set_position(get_position());
	sound.play();
}

void ClientPlayerPawn::tick(const GameTick &tick)
{
	if (is_owner)
	{
		cur_movement.key_forward.next_pressed = world()->game()->buttons.buttons["move-forward"].down;
		cur_movement.key_back.next_pressed = world()->game()->buttons.buttons["move-back"].down;
		cur_movement.key_left.next_pressed = world()->game()->buttons.buttons["move-left"].down;
		cur_movement.key_right.next_pressed = world()->game()->buttons.buttons["move-right"].down;
		cur_movement.key_jump.next_pressed = world()->game()->buttons.buttons["jump"].down;
		cur_movement.key_fire_primary.next_pressed = world()->game()->buttons.buttons["fire-primary"].down;
		cur_movement.key_fire_secondary.next_pressed = world()->game()->buttons.buttons["fire-secondary"].down;
		cur_movement.key_weapon = 0;
		if (world()->game()->buttons.buttons["switch-to-icelauncher"].down) cur_movement.key_weapon = 1;
		else if (world()->game()->buttons.buttons["switch-to-rocketlauncher"].down) cur_movement.key_weapon = 2;
		else if (world()->game()->buttons.buttons["switch-to-dualguns"].down) cur_movement.key_weapon = 3;
		else if (world()->game()->buttons.buttons["switch-to-sniperrifle"].down) cur_movement.key_weapon = 4;
		else if (world()->game()->buttons.buttons["switch-to-tractorbeam"].down) cur_movement.key_weapon = 5;

		NetGameEvent netevent("player-pawn-input");
		netevent.add_argument(NetGameEventValue(cur_movement.key_forward.next_pressed));
		netevent.add_argument(NetGameEventValue(cur_movement.key_back.next_pressed));
		netevent.add_argument(NetGameEventValue(cur_movement.key_left.next_pressed));
		netevent.add_argument(NetGameEventValue(cur_movement.key_right.next_pressed));
		netevent.add_argument(NetGameEventValue(cur_movement.key_jump.next_pressed));
		netevent.add_argument(NetGameEventValue(cur_movement.key_fire_primary.next_pressed));
		netevent.add_argument(NetGameEventValue(cur_movement.key_fire_secondary.next_pressed));
		netevent.add_argument(cur_movement.key_weapon);
		netevent.add_argument(cur_movement.dir);
		netevent.add_argument(cur_movement.up);

		//Console::write_line("Sent dir %1 to tick %2", cur_movement.dir, tick.arrival_tick_time);

		world()->game()->network->queue_event("server", netevent, tick.arrival_tick_time);

		PlayerPawnMovement past = cur_movement;
		past.tick_time = tick.arrival_tick_time;
		sent_movements.push_back(past);
	}

	bool was_moving = animation_move_speed > 0.0f;

	PlayerPawn::tick(tick);

	bool is_moving = animation_move_speed > 0.0f;

	if (!scene_object.is_null())
	{
		if (was_moving && !is_moving)
			scene_object.play_animation("static", true);
		else if (!was_moving && is_moving)
			scene_object.play_animation("run", true);
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

void ClientPlayerPawn::frame(float time_elapsed, float interpolated_time)
{
	float mouse_speed_x = 5.0f;
	float mouse_speed_y = 5.0f;

	float mouse_speed_multiplier = 1.0f;

	cur_movement.dir = std::remainder(cur_movement.dir + world()->mouse_movement.x * mouse_speed_x * mouse_speed_multiplier * time_elapsed, 360.0f);
	cur_movement.up = clamp(cur_movement.up + world()->mouse_movement.y * mouse_speed_y * mouse_speed_multiplier * time_elapsed, -90.0f, 90.0f);

	bool first_person_camera = true;

	Quaternionf look_dir = Quaternionf(cur_movement.up, cur_movement.dir, 0.0f, angle_degrees, order_YXZ);
	Vec3f look_pos = mix(last_position, next_position, interpolated_time) + eye_offset;

	float available_zoom_out = 0.0f;

	if (!first_person_camera)
	{
		float max_zoom_out = 15.0f;
		Vec3f ideal_look_pos = look_pos + look_dir.rotate_vector(Vec3f(0.0f, 0.0f, -max_zoom_out));

		int wall_hit_index = -1;
		camera_sweep_test.test_all_hits(camera_shape, look_pos, Quaternionf(), ideal_look_pos, Quaternionf());
		for (int i = 0; i < camera_sweep_test.get_hit_count(); i++)
		{
			std::shared_ptr<GameObjectCollision> obj_collision = camera_sweep_test.get_hit_object(i).get_data<GameObjectCollision>();
			if (!obj_collision)
			{
				wall_hit_index = i;
				break;
			}
		}

		if (wall_hit_index >= 0)
		{
			available_zoom_out = look_pos.distance(camera_sweep_test.get_hit_position(wall_hit_index));
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

	zoom_out = std::min(zoom_out + time_elapsed * 5.0f, available_zoom_out);

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
		if (scene_object.is_null())
		{
			SceneModel model(world()->game()->gc, world()->game()->scene, "Models/Thalania/Thalania.cmodel");
			scene_object = SceneObject(world()->game()->scene, model);
			scene_object.set_scale(Vec3f(0.075f));

			if (animation_move_speed > 0.0f)
				scene_object.play_animation("run", true);
			else
				scene_object.play_animation("static", true);
		}

		scene_object.set_position(mix(last_position, next_position, interpolated_time) + Vec3f(0.0f, 0.3f, 0.0f));
		scene_object.set_orientation(Quaternionf(clamp(-cur_movement.up * 0.5f, -15.0f, 15.0f), 180.0f + cur_movement.dir, 0.0f, angle_degrees, order_YXZ));
		//scene_object.update(time_elapsed);
		scene_object.moved(animation_move_speed * time_elapsed);
	}
	else
	{
		scene_object = SceneObject();
	}

	camera.set_position(look_pos);
	camera.set_orientation(look_dir);

	weapon->frame(time_elapsed, interpolated_time);
}

/*

    // Find final mouse speed
    var speed = this.mouseSpeed * this.mouseSpeedMultiplier;

    this.setZoomMoveSpeed = function (fov, speed) {
        this.camera.setFieldOfView(fov);
        this.mouseSpeedMultiplier = speed;
    }

*/
