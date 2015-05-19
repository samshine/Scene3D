
#include "precomp.h"
#include "client_player_pawn.h"
#include "game_tick.h"
#include "game_object_collision.h"
#include "Model/game.h"
#include <algorithm>

using namespace clan;

ClientPlayerPawn::ClientPlayerPawn(GameWorld *world) : PlayerPawn(world)
{
	camera = SceneCamera(*world->game()->scene);
	camera_shape = Physics3DShape::sphere(1.0f);
	camera_sweep_test = Physics3DSweepTest(world->game()->collision);
}

ClientPlayerPawn::~ClientPlayerPawn()
{

}

void ClientPlayerPawn::net_create(const GameTick &tick, const clan::NetGameEvent &net_event)
{
	net_update(tick, net_event);

	if (is_owner)
		world()->game()->scene->set_camera(camera);
}

void ClientPlayerPawn::net_update(const GameTick &tick, const clan::NetGameEvent &net_event)
{
	is_owner = net_event.get_argument(1).get_boolean();

	PastMovement past;
	past.tick_time = tick.receive_tick_time;
	past.key_forward = net_event.get_argument(2).get_boolean();
	past.key_back = net_event.get_argument(3).get_boolean();
	past.key_left = net_event.get_argument(4).get_boolean();
	past.key_right = net_event.get_argument(5).get_boolean();
	past.key_jump = net_event.get_argument(6).get_boolean();
	past.key_fire_primary = net_event.get_argument(7).get_boolean();
	past.key_fire_secondary = net_event.get_argument(8).get_boolean();
	past.key_weapon = net_event.get_argument(9).get_integer();
	past.dir = net_event.get_argument(10).get_number();
	past.up = net_event.get_argument(11).get_number();
	past.pos.x = net_event.get_argument(12).get_number();
	past.pos.y = net_event.get_argument(13).get_number();
	past.pos.z = net_event.get_argument(14).get_number();
	past.move_velocity.x = net_event.get_argument(15).get_number();
	past.move_velocity.y = net_event.get_argument(16).get_number();
	past.move_velocity.z = net_event.get_argument(17).get_number();
	past.fly_velocity.x = net_event.get_argument(18).get_number();
	past.fly_velocity.y = net_event.get_argument(19).get_number();
	past.fly_velocity.z = net_event.get_argument(20).get_number();
	past.is_flying = net_event.get_argument(21).get_boolean();
	health = net_event.get_argument(22).get_number();

	if (!is_owner)
	{
		controller->set_position(past.pos);
		controller->set_fly_velocity(past.fly_velocity, past.is_flying);
		move_velocity = past.move_velocity;
		key_forward = past.key_forward;
		key_back = past.key_back;
		key_left = past.key_left;
		key_right = past.key_right;
		key_jump = past.key_jump;
		key_fire_primary = past.key_fire_primary;
		key_fire_secondary = past.key_fire_secondary;
		key_weapon = past.key_weapon;
		dir = past.dir;
		up = past.up;

		int predict_ticks = std::min(tick.arrival_tick_time - tick.receive_tick_time, 15) / 2;
		for (int i = 0; i < predict_ticks; i++)
			tick_controller_movement(tick.time_elapsed);
	}
	else
	{
		// Save current view dir
		float present_dir = dir;
		float present_up = up;

		// Remove obsolete entries
		while (!sent_movements.empty() && sent_movements.front().tick_time <= tick.receive_tick_time)
			sent_movements.erase(sent_movements.begin());
		
		// Swap actual movement with predicted movement
		if (!sent_movements.empty() && sent_movements.front().tick_time == past.tick_time)
		{
			sent_movements.front() = past;
		}
		else if (sent_movements.empty())
		{
			sent_movements.push_back(past);
		}

		// Restore state from the past
		controller->set_position(sent_movements.front().pos);
		controller->set_fly_velocity(sent_movements.front().fly_velocity, sent_movements.front().is_flying);
		move_velocity = sent_movements.front().move_velocity;

		// Replay movement based on input
		for (size_t i = 0; i < sent_movements.size(); i++)
		{
			if (i > 0) // Store new predicted movement
			{
				sent_movements[i].pos = controller->get_position();
				sent_movements[i].fly_velocity = controller->get_fly_velocity(sent_movements[i].is_flying);
				sent_movements[i].move_velocity = move_velocity;
			}

			key_forward = sent_movements[i].key_forward;
			key_back = sent_movements[i].key_back;
			key_left = sent_movements[i].key_left;
			key_right = sent_movements[i].key_right;
			key_jump = sent_movements[i].key_jump;
			dir = sent_movements[i].dir;
			up = sent_movements[i].up;

			tick_controller_movement(tick.time_elapsed);
		}

		// Restore current view dir
		dir = present_dir;
		up = present_up;
	}
}

void ClientPlayerPawn::net_hit(const GameTick &tick, const clan::NetGameEvent &net_event)
{
	sound = AudioObject(*world()->game()->audio.get());
	sound.set_sound("Resources/Baleout/Sound/shit01.wav");
	sound.set_attenuation_begin(1.0f);
	sound.set_attenuation_end(100.0f);
	sound.set_volume(0.75f);
	sound.set_position(get_position());
	sound.play();
}

void ClientPlayerPawn::tick(const GameTick &tick)
{
	if (is_owner)
	{
		key_forward = world()->game()->buttons.buttons["move-forward"].down;
		key_back = world()->game()->buttons.buttons["move-back"].down;
		key_left = world()->game()->buttons.buttons["move-left"].down;
		key_right = world()->game()->buttons.buttons["move-right"].down;
		key_jump = world()->game()->buttons.buttons["jump"].down;
		key_fire_primary = world()->game()->buttons.buttons["fire-primary"].down;
		key_fire_secondary = world()->game()->buttons.buttons["fire-secondary"].down;
		key_weapon = 0;
		if (world()->game()->buttons.buttons["switch-to-icelauncher"].down) key_weapon = 1;
		else if (world()->game()->buttons.buttons["switch-to-rocketlauncher"].down) key_weapon = 2;
		else if (world()->game()->buttons.buttons["switch-to-dualguns"].down) key_weapon = 3;
		else if (world()->game()->buttons.buttons["switch-to-sniperrifle"].down) key_weapon = 4;
		else if (world()->game()->buttons.buttons["switch-to-tractorbeam"].down) key_weapon = 5;

		NetGameEvent netevent("player-pawn-input");
		netevent.add_argument(NetGameEventValue(key_forward));
		netevent.add_argument(NetGameEventValue(key_back));
		netevent.add_argument(NetGameEventValue(key_left));
		netevent.add_argument(NetGameEventValue(key_right));
		netevent.add_argument(NetGameEventValue(key_jump));
		netevent.add_argument(NetGameEventValue(key_fire_primary));
		netevent.add_argument(NetGameEventValue(key_fire_secondary));
		netevent.add_argument(key_weapon);
		netevent.add_argument(dir);
		netevent.add_argument(up);

		world()->game()->network->queue_event("server", netevent, tick.arrival_tick_time);

		PastMovement past;
		past.tick_time = tick.arrival_tick_time;
		past.key_forward = key_forward;
		past.key_back = key_back;
		past.key_left = key_left;
		past.key_right = key_right;
		past.key_jump = key_jump;
		past.key_fire_primary = key_fire_primary;
		past.key_fire_secondary = key_fire_secondary;
		past.key_weapon = key_weapon;
		past.dir = dir;
		past.up = up;
		past.pos = controller->get_position();
		past.move_velocity = move_velocity;
		past.fly_velocity = controller->get_fly_velocity(past.is_flying);
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
		next_position = controller->get_position();
	}
	else
	{
		next_position = controller->get_position();
		last_position = next_position;
	}
}

void ClientPlayerPawn::frame(float time_elapsed, float interpolated_time)
{
	float mouse_speed = 0.05421f;
	float mouse_speed_multiplier = 1.0f;

	float speed = mouse_speed * mouse_speed_multiplier;

	dir = std::remainder(dir + world()->mouse_movement.x * speed, 360.0f);
	up = clamp(up + world()->mouse_movement.y * speed, -90.0f, 90.0f);

	bool first_person_camera = true;

	Quaternionf look_dir = Quaternionf(up, dir, tilt, angle_degrees, order_YXZ);
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
			SceneModel model(world()->game()->gc, *world()->game()->scene, "Thalania/Thalania.cmodel");
			scene_object = SceneObject(*world()->game()->scene, model);
			scene_object.set_scale(Vec3f(0.15f));

			if (animation_move_speed > 0.0f)
				scene_object.play_animation("run", true);
			else
				scene_object.play_animation("static", true);
		}

		scene_object.set_position(mix(last_position, next_position, interpolated_time) + Vec3f(0.0f, 0.3f, 0.0f));
		scene_object.set_orientation(Quaternionf(clamp(-up * 0.5f, -15.0f, 15.0f), 180.0f + dir, 0.0f, angle_degrees, order_YXZ));
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
