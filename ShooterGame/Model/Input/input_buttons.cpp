
#include "precomp.h"
#include "input_buttons.h"

using namespace uicore;

void InputButtons::input_disabled()
{
	for (std::map<std::string, InputButton>::iterator it = buttons.begin(); it != buttons.end(); ++it)
		it->second.down = false;
}

void InputButtons::update(const DisplayWindowPtr &ic)
{
	input_disabled();
	for (std::map<std::string, InputButton>::iterator it = buttons.begin(); it != buttons.end(); ++it)
	for (size_t i = 0; i < it->second.keycodes.size(); i++)
		it->second.down = it->second.down || ic->input_device(it->second.keycodes[i].device)->keycode(it->second.keycodes[i].id);
}

void InputButtons::load(const DisplayWindowPtr &window, JsonValue buttons_node)
{
	for (auto &button : buttons_node.items())
	{
		std::string name = button["name"].to_string();
		for (auto &key : button["keys"].items())
		{
			std::string device_name = key["device"].to_string();
			std::string id = key["id"].to_string();
			buttons[name].keycodes.push_back(InputKey(device_name, window->input_device(device_name)->string_to_keyid(id)));
		}
	}
}
