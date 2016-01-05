
#pragma once

class Settings
{
public:
	static float mouse_speed_x() { return instance()->_mouse_speed_x; }
	static void set_mouse_speed_x(float value) { instance()->_mouse_speed_x = value; instance()->save(); sig_changed(); }

	static float mouse_speed_y() { return instance()->_mouse_speed_y; }
	static void set_mouse_speed_y(float value) { instance()->_mouse_speed_y = value; instance()->save(); sig_changed(); }

	static const std::string &player_name() { return instance()->_player_name; }
	static void set_player_name(const std::string &value) { instance()->_player_name = value; instance()->save(); sig_changed(); }

	static const std::string &server() { return instance()->_server; }
	static void set_server(const std::string &value) { instance()->_server = value; instance()->save(); sig_changed(); }

	static const std::string &port() { return instance()->_port; }
	static void set_port(const std::string &value) { instance()->_port = value; instance()->save(); sig_changed(); }

	static uicore::Signal<void()> &sig_changed() { return instance()->_sig_changed; }

private:
	Settings();
	static Settings *instance();

	void load();
	void save();

	uicore::Signal<void()> _sig_changed;
	float _mouse_speed_x = 8.0f;
	float _mouse_speed_y = 8.0f;
	std::string _player_name = "Player1";
	std::string _server = "localhost";
	std::string _port = "5004";
};
