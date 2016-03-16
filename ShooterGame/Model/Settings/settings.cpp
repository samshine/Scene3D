
#include "precomp.h"
#include "settings.h"

using namespace uicore;

namespace
{
	std::string settings_filename()
	{
		auto appdata_path = Directory::appdata("ShooterGame", "ShooterGame", "1");
		return PathHelp::combine(appdata_path, "settings.json");
	}
}

Settings *Settings::instance()
{
	static Settings settings;
	return &settings;
}

Settings::Settings()
{
	load();
}

void Settings::load()
{
	try
	{
		auto text = File::read_all_text(settings_filename());
		auto json = JsonValue::parse(text);
		_mouse_speed_x = json["mouseSpeedX"].to_float();
		_mouse_speed_y = json["mouseSpeedY"].to_float();
		_player_name = json["playerName"].to_string();
		_server = json["server"].to_string();
		_port = json["port"].to_string();
	}
	catch (...)
	{
	}
	_sig_changed();
}

void Settings::save()
{
	try
	{
		JsonValue json = JsonValue::object();
		json["mouseSpeedX"].set_number(_mouse_speed_x);
		json["mouseSpeedY"].set_number(_mouse_speed_y);
		json["playerName"].set_string(_player_name);
		json["server"].set_string(_server);
		json["port"].set_string(_port);

		File::write_all_text(settings_filename(), json.to_json());
	}
	catch (...)
	{
	}
}
