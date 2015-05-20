
#pragma once

#include "Model/Audio/music_player.h"
#include "Model/Network/game_network.h"
#include "Model/Network/lock_step_time.h"
#include "Model/GameWorld/game_world.h"
#include "Model/Input/input_buttons.h"

class Game
{
public:
	Game(std::string hostname, std::string port, bool server);

#if 0
	void run_client(Game *server = 0);
	void run_server(clan::Event &stop_event);
#endif

	//std::unique_ptr<UIScreenManager> screen_manager;
	//std::unique_ptr<GameScreen> game_screen;
	clan::GraphicContext gc;

	std::unique_ptr<clan::Scene> scene;

	clan::SoundOutput sound_output;
	std::unique_ptr<clan::AudioWorld> audio;
	std::unique_ptr<MusicPlayer> music_player;

	InputButtons buttons;

	std::shared_ptr<GameNetwork> network;
	std::unique_ptr<LockStepTime> lock_step_time;

	clan::Physics3DWorld collision;

	std::string map_basepath;
	std::string map_name;

	clan::DomDocument xml;
	clan::JsonValue level_data;

	clan::SceneObject level_instance;
	std::vector<clan::SceneObject> objects;

	std::vector<clan::Physics3DObject> level_collision_objects;

	std::unique_ptr<GameWorld> game_world;

#if 0
private:
	void create_client_objects();
	void create_scene_objects();
	void create_input_buttons();
	//void init_script();
	//void load_script_nodes(const std::string &xpath, const std::string &base_path);
	void update();

	void on_game_init(bool is_server);
	void on_frame_update(float time_elapsed, float interpolated_time);
	void on_game_tick(float time_elapsed, int receive_tick_time, int arrival_tick_time);
	void on_net_peer_connected(const std::string &id);
	void on_net_peer_disconnected(const std::string &id);
	void on_net_event_received(const std::string &sender, const clan::NetGameEvent &net_event);

	//	std::vector<ScriptValue> create_args(const std::string &sender, const clan::NetGameEvent &net_event, int start_argument);

#endif
	bool server;
#if 0
	//ElapsedTimer elapsed_timer;
	//ScriptContext context;

	clan::SlotContainer slots;
#endif
};
