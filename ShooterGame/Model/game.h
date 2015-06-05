
#pragma once

#include "Model/Audio/music_player.h"
#include "Model/Network/game_network.h"
#include "Model/Network/lock_step_time.h"
#include "Model/GameWorld/game_world.h"
#include "Model/Input/input_buttons.h"

class Game
{
public:
	Game(std::string hostname, std::string port, bool server, clan::ResourceManager resources = clan::ResourceManager(), clan::GraphicContext gc = clan::GraphicContext(), clan::InputContext ic = clan::InputContext());

	void update(clan::Vec2i mouse_movement);

	clan::ResourceManager resources;
	clan::GraphicContext gc;
	clan::InputContext ic;

	clan::Scene scene;

	std::unique_ptr<clan::AudioWorld> audio;
	std::unique_ptr<MusicPlayer> music_player;

	InputButtons buttons;

	std::shared_ptr<GameNetwork> network;
	std::unique_ptr<LockStepTime> lock_step_time;

	clan::Physics3DWorld collision;

	clan::JsonValue level_data;

	clan::SceneObject level_instance;
	std::vector<clan::SceneObject> objects;

	std::vector<clan::Physics3DObject> level_collision_objects;

	std::unique_ptr<GameWorld> game_world;

private:
	void create_client_objects();
	void create_scene_objects();
	void create_input_buttons();

	void on_game_init(bool is_server);
	void on_frame_update(float time_elapsed, float interpolated_time);
	void on_game_tick(float time_elapsed, int receive_tick_time, int arrival_tick_time);
	void on_net_peer_connected(const std::string &id);
	void on_net_peer_disconnected(const std::string &id);
	void on_net_event_received(const std::string &sender, const clan::NetGameEvent &net_event);

	bool server;
	clan::GameTime elapsed_timer;
	std::string map_cmodel_filename;
	clan::SlotContainer slots;
};
