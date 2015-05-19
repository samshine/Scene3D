
#include "precomp.h"
#include "game.h"
#include "Model/Audio/sound_cache_provider.h"
#include "Model/Network/game_network_client.h"
#include "Model/Network/game_network_server.h"
#include "Model/Network/lock_step_client_time.h"
#include "Model/Network/lock_step_server_time.h"

using namespace clan;

Game::Game(std::string hostname, std::string port, bool server)
: server(server)
{
	map_basepath = "Resources/Baleout/Scene";
	map_name = "scene";

	xml = DomDocument(File(PathHelp::combine(map_basepath, map_name + ".xml")));
	level_data = JsonValue::from_json(File::read_text(PathHelp::combine(map_basepath, map_name + ".json")));

	if (!xml.select_node("/scene/level").to_element().get_attribute_bool("no-collision"))
		level_collision_objects.push_back(Physics3DObject::rigid_body(
			collision,
			Physics3DShape::scale_model(
				Physics3DShape::model(
					ModelData::load(
						PathHelp::combine(
						map_basepath,
							xml.select_string("/scene/level/model/filename/text()")))),
						Vec3f(xml.select_float("/scene/level/scale/text()")))));

	std::map<std::string, Physics3DShape> level_shapes;

	std::vector<DomNode> object_nodes = xml.select_nodes("/scene/object");
	for (size_t i = 0; i < object_nodes.size(); i++)
	{
		if (object_nodes[i].to_element().get_attribute_bool("no-collision"))
			continue;

		Vec3f position(
			object_nodes[i].select_float("position/x/text()"),
			object_nodes[i].select_float("position/y/text()"),
			object_nodes[i].select_float("position/z/text()"));
		Vec3f scale(object_nodes[i].select_float("scale/text()"));
		Vec3f rotate(
			object_nodes[i].select_float("orientation/direction/text()"),
			object_nodes[i].select_float("orientation/up/text()"),
			object_nodes[i].select_float("orientation/tilt/text()"));
		std::string model_name = object_nodes[i].select_string("model/filename/text()");

		auto it = level_shapes.find(model_name);
		if (it == level_shapes.end())
		{
			std::shared_ptr<ModelData> model_data = ModelData::load(PathHelp::combine(map_basepath, model_name));

			level_shapes[model_name] = Physics3DShape::model(model_data);
			it = level_shapes.find(model_name);
		}

		level_collision_objects.push_back(Physics3DObject::rigid_body(collision, Physics3DShape::scale_model(it->second, scale), 0.0f, position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ)));
	}

	for (auto obj : level_collision_objects)
	{
		obj.set_static(true);
	}

	if (server)
		network.reset(new GameNetworkServer());
	else
		network.reset(new GameNetworkClient());

	slots.connect(network->sig_peer_connected, this, &Game::on_net_peer_connected);
	slots.connect(network->sig_peer_disconnected, this, &Game::on_net_peer_disconnected);
	slots.connect(network->sig_event_received, this, &Game::on_net_event_received);

	if (server)
		lock_step_time.reset(new LockStepServerTime(network));
	else
		lock_step_time.reset(new LockStepClientTime(network));

	network->start(hostname, port);

	game_world.reset(new GameWorld(this));

	if (!server)
		create_client_objects();

	on_game_init(server);
}

void Game::create_client_objects()
{
	ScreenInfo screen_info;
	int primary_screen_index = 0;
	std::vector<Rectf> screen_boxes = screen_info.get_screen_geometries(primary_screen_index);

	DisplayWindowDescription desc;
	desc.set_title("Baleout!");
	desc.set_size(Sizef(screen_boxes[primary_screen_index].get_width() * 90 / 100, (screen_boxes[primary_screen_index].get_height() - 50) * 90 / 100), true);
	desc.set_allow_resize(true);
	desc.set_visible(false);

	screen_manager.reset(new UIScreenManager(desc, "Resources/GameIDE/Icons/gameide-48.png", "Resources/GameIDE/Icons/gameide-16.png"));
	screen_manager->hide_cursor();
	screen_manager->maximize();

	GraphicContext gc = screen_manager->get_window().get_gc();

	ResourceManager resources;
	SceneCache::set(resources, std::make_shared<CacheProvider>(gc));
	SoundCache::set(resources, std::make_shared<SoundCacheProvider>());

	sound_output = SoundOutput(44100);
	audio.reset(new AudioWorld(resources));
	music_player.reset(new MusicPlayer());

	scene.reset(new Scene(gc, resources, "Resources/Scene3D"));

	std::vector<Colorf> colors;
	colors.push_back(Colorf(0.001f, 0.002f, 0.02f, 1.0f));
	colors.push_back(Colorf(0.001f, 0.002f, 0.01f, 1.0f));
	colors.push_back(Colorf(0.001f, 0.002f, 0.005f, 1.0f));
	colors.push_back(Colorf(0.001f, 0.002f, 0.01f, 1.0f));
	colors.push_back(Colorf(0.001f, 0.002f, 0.02f, 1.0f));
	scene->set_skybox_gradient(gc, colors);

	game_screen.reset(new GameScreen(this));

	create_scene_objects();
	create_input_buttons();
}

void Game::create_scene_objects()
{
	GraphicContext gc = screen_manager->get_window().get_gc();

	std::vector<DomNode> object_nodes = xml.select_nodes("/scene/object");
	objects.reserve(object_nodes.size());
	for (size_t i = 0; i < object_nodes.size(); i++)
	{
		if (object_nodes[i].to_element().get_attribute_bool("no-render"))
			continue;

		Vec3f position(
			object_nodes[i].select_float("position/x/text()"),
			object_nodes[i].select_float("position/y/text()"),
			object_nodes[i].select_float("position/z/text()"));
		Vec3f scale(object_nodes[i].select_float("scale/text()"));
		Vec3f rotate(
			object_nodes[i].select_float("orientation/direction/text()"),
			object_nodes[i].select_float("orientation/up/text()"),
			object_nodes[i].select_float("orientation/tilt/text()"));
		std::string model_name = object_nodes[i].select_string("model/filename/text()");
		std::string animation_name = object_nodes[i].select_string("animation/name/text()");
		objects.push_back(SceneObject(*scene, SceneModel(gc, *scene, model_name), position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ), scale));
		objects.back().play_animation(animation_name);
	}

	level_instance = SceneObject(*scene, SceneModel(gc, *scene, xml.select_string("/scene/level/model/filename/text()")), Vec3f(), Quaternionf(), Vec3f(xml.select_float("/scene/level/scale/text()")));
}

void Game::create_input_buttons()
{
	DomDocument xml(File("Resources/Baleout/Scene/input.xml"));

	InputContext ic = screen_manager->get_window().get_ic();
	buttons.load(ic, xml.select_node("/input/buttons"));
}

void Game::run_server(clan::Event &stop_event)
{
	srand((unsigned int)System::get_time());

	ConsoleWindow window("Baleout server");
	Console::write_line("Press Ctrl+C to stop the Baleout server");
	while (!stop_event.wait(0))
	{
		update();
		KeepAlive::process(5);
	}
}

void Game::run_client(Game *server)
{
	try
	{
		bool exit = false;
		Slot slot = screen_manager->get_window().sig_window_close().connect_functor([&exit] { exit = true; });

		screen_manager->get_window().show();

		game_screen->set_active();
		game_screen->enable_mouse(false);

		while (!exit)
		{
			if (server)
				server->update();
			update();
			screen_manager->update();
			KeepAlive::process();
		}

		screen_manager->get_window().hide();
	}
	catch (Exception &e)
	{
		ExceptionDialog::show(e/*, screen_manager->get_window().get_hwnd()*/);
	}
}

void Game::update()
{
	ScopeTimeFunction();

	lock_step_time->update();

	float time_elapsed = elapsed_timer.update();

	int ticks = lock_step_time->get_ticks_elapsed();
	for (int i = 0; i < ticks; i++)
	{
		int receive_tick_time = lock_step_time->get_receive_tick_time() + i;
		int arrival_tick_time = lock_step_time->get_arrival_tick_time() + i;
		float tick_time_elapsed = lock_step_time->get_tick_time_elapsed();

		collision.step_simulation_once(tick_time_elapsed);
		game_world->net_tick = GameTick(tick_time_elapsed, receive_tick_time, arrival_tick_time);
		network->receive_events(receive_tick_time);
		on_game_tick(tick_time_elapsed, receive_tick_time, arrival_tick_time);
	}

	if (!server)
	{
		level_instance.update(time_elapsed);
		for (size_t i = 0; i < objects.size(); i++)
			objects[i].update(time_elapsed);
	}

	if (!server)
	{
		on_frame_update(time_elapsed, lock_step_time->get_tick_interpolation_time());

		music_player->update();
		audio->set_listener(scene->get_camera().get_position(), scene->get_camera().get_orientation());
		audio->update();
	}
}

void Game::on_game_init(bool is_server)
{
	game_world->init(is_server);
}

void Game::on_frame_update(float time_elapsed, float interpolated_time)
{
	game_world->frame(time_elapsed, interpolated_time);
}

void Game::on_game_tick(float time_elapsed, int receive_tick_time, int arrival_tick_time)
{
	game_world->tick(time_elapsed, receive_tick_time, arrival_tick_time);
}

void Game::on_net_peer_connected(const std::string &id)
{
	game_world->net_peer_connected(id);
}

void Game::on_net_peer_disconnected(const std::string &id)
{
	game_world->net_peer_disconnected(id);
}

void Game::on_net_event_received(const std::string &sender, const clan::NetGameEvent &net_event)
{
	game_world->net_event_received(sender, net_event);
}
