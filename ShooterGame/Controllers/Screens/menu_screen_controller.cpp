
#include "precomp.h"
#include "menu_screen_controller.h"
#include "game_screen_controller.h"
#include "Model/Settings/settings.h"

using namespace uicore;

MenuScreenController::MenuScreenController()
{
	scene = Scene::create(scene_engine());
	scene_camera = SceneCamera::create(scene);
	scene_camera->set_position(Vec3f(0.0f, 1.8f, -3.0f));
	scene_camera->set_orientation(Quaternionf(10.0f, 22.0f, 4.0f, angle_degrees, order_YXZ));

	scene->show_skybox_stars(false);
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
		g.x = std::pow(g.x, 2.2f);
		g.y = std::pow(g.y, 2.2f);
		g.z = std::pow(g.z, 2.2f);
	}
	scene->set_skybox_gradient(gradient);

	auto map_data = MapData::load("Resources/Assets/Levels/Liandri/liandri.cmap");
	for (const auto &item : map_data->objects)
	{
		if (item.type != "Static" && item.type != "Level")
			continue;

		Vec3f position = item.position;
		Vec3f scale = Vec3f(item.scale);
		Vec3f rotate(item.dir, item.up, item.tilt);
		std::string model_name = item.mesh;
		std::string animation_name = item.animation;
		scene_objects.push_back(SceneObject::create(scene, SceneModel::create(scene, model_name), position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ), scale));
		scene_objects.back()->play_animation(animation_name, true);
	}

	music_player.play("Resources/Assets/Music/menu.ogg", true);

	create_menus();
}

void MenuScreenController::create_menus()
{
	auto player_name = []() -> std::string { return Settings::player_name(); };
	auto set_player_name = [](std::string value) { Settings::set_player_name(value); };

	auto server = []() -> std::string { return Settings::server(); };
	auto set_server = [](std::string value) { Settings::set_server(value); };

	auto port = []() -> std::string { return Settings::port(); };
	auto set_port = [](std::string value) { Settings::set_port(value); };

	main_menu = {
		{ "New Game", [=]() { music_player.stop(); present_controller(std::make_shared<GameScreenController>("localhost", "5004", true)); } },
		{ "Join Game", [=]() { game_menu_view->push_menu(&join_menu); } },
		{ "Host Game", [=]() { game_menu_view->push_menu(&host_menu); } },
		{ "Options", [=]() { game_menu_view->push_menu(&options_menu); } },
		{ "Quit", [=]() { exit_game(); } }
	};

	join_menu = {
		{ "Join Game", [=]() { music_player.stop(); present_controller(std::make_shared<GameScreenController>(server(), port(), false)); } },
		{ "Player Name:", [=]() { game_menu_view->begin_edit(player_name(), set_player_name); }, player_name },
		{ "Server:", [=]() { game_menu_view->begin_edit(server(), set_server); }, server },
		{ "Port:", [=]() { game_menu_view->begin_edit(port(), set_port); }, port },
		{ "Back", [=]() { game_menu_view->pop_menu(); } }
	};

	host_menu = {
		{ "Create Game", [=]() { music_player.stop(); present_controller(std::make_shared<GameScreenController>("", port(), true)); } },
		{ "Player Name:", [=]() { game_menu_view->begin_edit(player_name(), set_player_name); }, player_name },
		{ "Port:", [=]() { game_menu_view->begin_edit(port(), set_port); }, port },
		{ "Back", [=]() { game_menu_view->pop_menu(); } }
	};

	options_menu = game_menu_view->create_options_menu();

	game_menu_view->push_menu(&main_menu);
}

void MenuScreenController::update()
{
	if (!game_menu_view->menu_visible())
		exit_game();

	fade_time = std::min(fade_time + game_time().time_elapsed() * 2.0f, 1.0f);

	t = std::fmod(t + game_time().time_elapsed() * 0.01f, 2.0f);

	float t2 = t > 1.0f ? 2.0f - t : t;
	scene_camera->set_position(Vec3f(-12.0f, 2.5f + 1.8f, -12.0f - 3.0f * t2));

	scene_viewport()->set_camera(scene_camera);
	for (const auto &item : scene_objects)
		item->update(game_time().time_elapsed());
	scene_viewport()->update(gc(), game_time().time_elapsed());
	scene_viewport()->render(gc());

	canvas()->begin();

	Path::rect(canvas()->size())->fill(canvas(), Brush::solid(Colorf(0.0f, 0.0f, 0.0f, 0.2f)));

	game_menu_view->update();

	if (fade_time < 1.0f)
		Path::rect(canvas()->size())->fill(canvas(), Brush::solid(Colorf(0.0f, 0.0f, 0.0f, clamp(1.0f - fade_time, 0.0f, 1.0f))));

	canvas()->end();
}
