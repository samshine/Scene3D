
#include "precomp.h"
#include "input_buttons.h"

using namespace clan;

void InputButtons::input_disabled()
{
	for (std::map<std::string, InputButton>::iterator it = buttons.begin(); it != buttons.end(); ++it)
		it->second.down = false;
}

void InputButtons::update(InputContext &ic)
{
	input_disabled();
	for (std::map<std::string, InputButton>::iterator it = buttons.begin(); it != buttons.end(); ++it)
	for (size_t i = 0; i < it->second.keycodes.size(); i++)
		it->second.down = it->second.down || ic.get_device(it->second.keycodes[i].device).get_keycode(it->second.keycodes[i].id);
}

void InputButtons::load(InputContext &ic, DomNode buttons_node)
{
	std::vector<DomNode> button_nodes = buttons_node.select_nodes("button");
	for (size_t i = 0; i < button_nodes.size(); i++)
	{
		std::string name = button_nodes[i].select_string("name/text()");
		InputButton &button = buttons[name];
		std::vector<DomNode> keys = button_nodes[i].select_nodes("key");
		for (size_t j = 0; j < keys.size(); j++)
		{
			std::string device_name = keys[j].select_string("device/text()");
			std::string id = keys[j].select_string("id/text()");
			button.keycodes.push_back(InputKey(device_name, ic.get_device(device_name).string_to_keyid(id)));
		}
	}
}
