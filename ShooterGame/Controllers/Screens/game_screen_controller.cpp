
#include "precomp.h"
#include "game_screen_controller.h"

using namespace uicore;

GameScreenController::GameScreenController(const CanvasPtr &canvas) : ScreenViewController(canvas)
{
}

void GameScreenController::update_desktop(const uicore::CanvasPtr &canvas, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta)
{
	if (desktop_exception_flag)
	{
		canvas->clear();
		return;
	}

	try
	{
		if (!client_game)
		{
			std::string hostname = "localhost";
			std::string port = "5004";

			GraphicContextPtr gc = canvas->gc();

			server_game = std::make_unique<Game>(hostname, port, true);
			client_game = std::make_unique<Game>(hostname, port, false, Screen::scene_cache(), Screen::sound_cache(), gc, ic);
		}

		server_game->update(mouse_delta);
		client_game->update(mouse_delta);

		render_scene(canvas, client_game->scene);
	}
	catch (...)
	{
		desktop_exception_flag = true;
		throw;
	}
}


/*
GameScreenController::GameScreenController(const CanvasPtr &canvas) : ScreenViewController(canvas), game_time(60), character_controller(collision_world)
{
	GraphicContextPtr gc = canvas.get_gc();

	scene = create_scene(canvas);
	scene.set_camera(SceneCamera(scene));

	scene.show_skybox_stars(false);
	std::vector<Colorf> gradient;
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 6 / 10, 240 * 6 / 10, 243 * 6 / 10));
	gradient.push_back(Colorf(236 * 7 / 10, 240 * 7 / 10, 243 * 7 / 10));
	gradient.push_back(Colorf(236 * 8 / 10, 240 * 8 / 10, 243 * 8 / 10));
	for (auto &g : gradient)
	{
		g.r = std::pow(g.r, 2.2f);
		g.g = std::pow(g.g, 2.2f);
		g.b = std::pow(g.b, 2.2f);
	}
	scene.set_skybox_gradient(gc, gradient);

	auto model_data = ModelData::load(PathHelp::combine("Resources/Assets", "Liandri/liandri.cmodel"));

	SceneModel model(gc, scene, model_data);
	map_object = SceneObject(scene, model);

	map_collision = Physics3DObject::collision_body(collision_world, Physics3DShape::model(model_data));
}

void GameScreenController::update_desktop(const uicore::CanvasPtr &canvas, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta)
{
	update_look_dir(mouse_delta);
	update_game(ic);
	update_camera();
	render_scene(canvas, scene);
}

void GameScreenController::update_game(const uicore::DisplayWindowPtr &ic)
{
	game_time.update();

	for (int tick = 0; tick < game_time.get_ticks_elapsed(); tick++)
	{
		game_input.update_buttons(ic);
		update_character_controller();
	}
}

void GameScreenController::update_look_dir(const uicore::Vec2i &mouse_delta)
{
	EulerRotation rotation = character_controller.get_rotation();
	float time_elapsed = game_time.get_time_elapsed();

	float mouse_speed_x = 5.0f;
	float mouse_speed_y = 5.0f;

	float dir_speed = 50.0f;
	if (game_input.button(keycode_left).pressed())
	{
		rotation.dir = std::fmod(rotation.dir - time_elapsed * dir_speed, 360.0f);
	}
	else if (game_input.button(keycode_right).pressed())
	{
		rotation.dir = std::fmod(rotation.dir + time_elapsed * dir_speed, 360.0f);
	}

	float up_speed = 50.0f;
	if (game_input.button(keycode_up).pressed())
	{
		rotation.up = clamp(rotation.up - time_elapsed * up_speed, -90.0f, 90.0f);
	}
	else if (game_input.button(keycode_down).pressed())
	{
		rotation.up = clamp(rotation.up + time_elapsed * up_speed, -90.0f, 90.0f);
	}

	rotation.dir = std::fmod(rotation.dir + mouse_delta.x * time_elapsed * mouse_speed_x, 360.0f);
	rotation.up = clamp(rotation.up + mouse_delta.y * time_elapsed * mouse_speed_y, -90.0f, 90.0f);

	character_controller.look(rotation);
}

void GameScreenController::update_camera()
{
	Physics3DSweepTest sweep_test(collision_world);
	Physics3DShape sphere_shape = Physics3DShape::sphere(0.25f);

	Quaternionf camera_orientation = character_controller.get_rotation().to_quaternionf();

	float zoom_out = 0.0f;// 3.0f;
	Vec3f camera_look_pos = character_controller.get_position() + Vec3f(0.0f, 1.8f, 0.0f) + camera_orientation.rotate_vector(Vec3f(0.0f, 0.0f, -0.5f));
	Vec3f camera_pos = camera_look_pos + camera_orientation.rotate_vector(Vec3f(0.0f, 0.0f, -zoom_out));

	if (sweep_test.test_first_hit(sphere_shape, camera_look_pos, Quaternionf(), camera_pos, Quaternionf()))
	{
		camera_pos = sweep_test.get_hit_position(0);
	}

	SceneCamera camera = scene.get_camera();
	camera.set_orientation(camera_orientation);
	camera.set_position(camera_pos);
}

void GameScreenController::update_character_controller()
{
	EulerRotation rotation = character_controller.get_rotation();

	Vec2f thrust;
	if (game_input.button(keycode_a).pressed() || game_input.button(keycode_q).pressed())
		thrust.x -= 1.0f;
	if (game_input.button(keycode_d).pressed())
		thrust.x += 1.0f;
	if (game_input.button(keycode_w).pressed() || game_input.button(keycode_z).pressed())
		thrust.y += 1.0f;
	if (game_input.button(keycode_s).pressed())
		thrust.y -= 1.0f;
	thrust.normalize();

	std::string anim = "default";
	if (thrust.x < -0.1f)
		anim = "strafe-left";
	else if (thrust.x > 0.1f)
		anim = "strafe-right";
	else if (thrust.y > 0.1f)
		anim = "forward";
	else if (thrust.y < -0.1f)
		anim = "backward";

	if (game_input.button(keycode_space).clicked() && !character_controller.is_flying())
	{
		character_controller.apply_impulse(Vec3f(0.0f, 4.0f, 0.0f));
		anim = "jump";
	}

	dodge_cooldown = std::max(dodge_cooldown - game_time.get_time_elapsed(), 0.0f);
	double_tap_left_elapsed += game_time.get_time_elapsed();
	double_tap_right_elapsed += game_time.get_time_elapsed();
	double_tap_up_elapsed += game_time.get_time_elapsed();
	double_tap_down_elapsed += game_time.get_time_elapsed();

	if (dodge_cooldown == 0.0f)
	{
		bool flying = character_controller.is_flying();
		if (double_tap_up_elapsed < 0.3f && (game_input.button(keycode_w).pressed() || game_input.button(keycode_z).pressed()) && !was_down_up && !flying)
		{
			Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(0.0f, 0.32f, 1.20f) * 7.0f));
			dodge_cooldown = 0.75f;
			anim = "dodge-forward";
		}

		if (double_tap_down_elapsed < 0.3f && game_input.button(keycode_s).pressed() && !was_down_down && !flying)
		{
			Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(0.0f, 0.32f, -1.20f) * 7.0f));
			dodge_cooldown = 0.75f;
			anim = "dodge-backward";
		}

		if (double_tap_left_elapsed < 0.3f && (game_input.button(keycode_a).pressed() || game_input.button(keycode_q).pressed()) && !was_down_left && !flying)
		{
			Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(-1.20f, 0.32f, 0.0f) * 7.0f));
			dodge_cooldown = 0.75f;
			anim = "dodge-left";
		}

		if (double_tap_right_elapsed < 0.3f && game_input.button(keycode_d).pressed() && !was_down_right && !flying)
		{
			Quaternionf move_direction(0.0f, rotation.dir, 0.0f, angle_degrees, order_YXZ);

			character_controller.apply_impulse(move_direction.rotate_vector(Vec3f(1.20f, 0.32f, 0.0f) * 7.0f));
			dodge_cooldown = 0.75f;
			anim = "dodge-right";
		}
	}

	if ((game_input.button(keycode_a).pressed() || game_input.button(keycode_q).pressed()) && !was_down_left) double_tap_left_elapsed = 0.0f;
	if (game_input.button(keycode_d).pressed() && !was_down_right) double_tap_right_elapsed = 0.0f;
	if ((game_input.button(keycode_w).pressed() || game_input.button(keycode_z).pressed()) && !was_down_up) double_tap_up_elapsed = 0.0f;
	if (game_input.button(keycode_s).pressed() && !was_down_down) double_tap_down_elapsed = 0.0f;

	was_down_left = (game_input.button(keycode_a).pressed() || game_input.button(keycode_q).pressed());
	was_down_right = game_input.button(keycode_d).pressed();
	was_down_up = (game_input.button(keycode_w).pressed() || game_input.button(keycode_z).pressed());
	was_down_down = game_input.button(keycode_s).pressed();

	if (!model_object.is_null() && anim != last_anim && !character_controller.is_flying())
	{
		model_object.play_animation(anim, false);
		last_anim = anim;
	}

	character_controller.thrust(thrust);

	if (map_collision.is_null())
	{
		character_controller.warp(Vec3f(0.0f, 0.2f, 0.0f), character_controller.get_rotation(), Vec3f());
	}
	else
	{
		character_controller.update(game_time.get_tick_time_elapsed());
	}
}
*/
