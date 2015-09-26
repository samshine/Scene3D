
#include "precomp.h"
#include "game.h"
#include "Model/Network/game_network_client.h"
#include "Model/Network/game_network_server.h"
#include "Model/Network/lock_step_client_time.h"
#include "Model/Network/lock_step_server_time.h"

using namespace uicore;

Game::Game(std::string hostname, std::string port, bool server, SceneCache scene_cache, const std::shared_ptr<SoundCache> &sound_cache, uicore::GraphicContext gc, uicore::DisplayWindow ic) : server(server), scene_cache(scene_cache), gc(gc), ic(ic)
{
	game_data = JsonValue::from_json(File::read_text("Resources/Config/game.json"));

	std::string map_name = game_data["map"];

	level_data = JsonValue::from_json(File::read_text(PathHelp::combine("Resources", map_name + ".mapdesc")));
	map_cmodel_filename = map_name + ".cmodel";

	level_collision_objects.push_back(Physics3DObject::rigid_body(collision, Physics3DShape::model(ModelData::load(PathHelp::combine("Resources", map_cmodel_filename)))));

	std::map<std::string, Physics3DShape> level_shapes;

	for (auto &item : level_data["objects"].get_items())
	{
		if (item["type"].to_string() != "Static")
			continue;

		Vec3f position(item["position"]["x"], item["position"]["y"], item["position"]["z"]);
		Vec3f scale(item["scale"]);
		Vec3f rotate(item["dir"], item["up"], item["tilt"]);
		std::string model_name = item["mesh"];

		auto it = level_shapes.find(model_name);
		if (it == level_shapes.end())
		{
			std::shared_ptr<ModelData> model_data = ModelData::load(PathHelp::combine("Resources", model_name));

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
		create_client_objects(sound_cache);

	on_game_init(server);
}

void Game::create_client_objects(const std::shared_ptr<SoundCache> &sound_cache)
{
	audio.reset(new AudioWorld(sound_cache));
	music_player.reset(new MusicPlayer());

	if (game_data["music"].to_boolean())
	{
		music_player->play(
		{
			"Resources/Music/game1.ogg",
			"Resources/Music/game2.ogg",
			"Resources/Music/game3.ogg"
		}, true);
	}

	scene = Scene(scene_cache);

	std::vector<Colorf> colors;
	colors.push_back(Colorf(0.001f, 0.002f, 0.02f, 1.0f));
	colors.push_back(Colorf(0.001f, 0.002f, 0.01f, 1.0f));
	colors.push_back(Colorf(0.001f, 0.002f, 0.005f, 1.0f));
	colors.push_back(Colorf(0.001f, 0.002f, 0.01f, 1.0f));
	colors.push_back(Colorf(0.001f, 0.002f, 0.02f, 1.0f));
	scene.set_skybox_gradient(gc, colors);

	create_scene_objects();
	create_input_buttons();
}

void Game::create_scene_objects()
{
	for (auto &item : level_data["objects"].get_items())
	{
		if (item["type"].to_string() != "Static" || item["fields"]["no_render"])
			continue;

		Vec3f position(item["position"]["x"], item["position"]["y"], item["position"]["z"]);
		Vec3f scale(item["scale"]);
		Vec3f rotate(item["dir"], item["up"], item["tilt"]);
		std::string model_name = item["mesh"];
		std::string animation_name = item["animation"];
		objects.push_back(SceneObject(scene, SceneModel(gc, scene, model_name), position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ), scale));
		objects.back().play_animation(animation_name, true);
	}

	level_instance = SceneObject(scene, SceneModel(gc, scene, map_cmodel_filename), Vec3f(), Quaternionf(), Vec3f(1.0f));
}

void Game::create_input_buttons()
{
	auto json = JsonValue::from_json(File::read_text("Resources/Config/input.json"));
	buttons.load(ic, json["buttons"]);
}

void Game::update(uicore::Vec2i mouse_movement)
{
	ScopeTimeFunction();

	game_world->mouse_movement = mouse_movement;
	buttons.update(ic);

	network->update();

	lock_step_time->update();
	elapsed_timer.update();

	float time_elapsed = elapsed_timer.get_time_elapsed();

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
		audio->set_listener(scene.get_camera().get_position(), scene.get_camera().get_orientation());
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

void Game::on_net_event_received(const std::string &sender, const uicore::NetGameEvent &net_event)
{
	game_world->net_event_received(sender, net_event);
}
