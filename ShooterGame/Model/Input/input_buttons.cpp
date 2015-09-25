
#include "precomp.h"
#include "input_buttons.h"

using namespace uicore;

void InputButtons::input_disabled()
{
	for (std::map<std::string, InputButton>::iterator it = buttons.begin(); it != buttons.end(); ++it)
		it->second.down = false;
}

void InputButtons::update(DisplayWindow &ic)
{
	input_disabled();
	for (std::map<std::string, InputButton>::iterator it = buttons.begin(); it != buttons.end(); ++it)
	for (size_t i = 0; i < it->second.keycodes.size(); i++)
		it->second.down = it->second.down || ic.get_input_device(it->second.keycodes[i].device).get_keycode(it->second.keycodes[i].id);
}

void InputButtons::load(DisplayWindow &window, JsonValue buttons_node)
{
	for (auto &button : buttons_node.get_items())
	{
		std::string name = button["name"].to_string();
		for (auto &key : button["keys"].get_items())
		{
			std::string device_name = key["device"];
			std::string id = key["id"];
			buttons[name].keycodes.push_back(InputKey(device_name, window.get_input_device(device_name).string_to_keyid(id)));
		}
	}
}
