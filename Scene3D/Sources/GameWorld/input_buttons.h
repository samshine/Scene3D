
#pragma once

#include <string>
#include <vector>
#include <map>

class InputKey
{
public:
	InputKey(const std::string &device, int id) : device(device), id(id) { }
	std::string device;
	int id;
};

class InputButton
{
public:
	InputButton() : down(false) { }

	std::vector<InputKey> keycodes;
	bool down;
};

class InputButtons
{
public:
	std::map<std::string, InputButton> buttons;

	void input_disabled();
	void update(const uicore::DisplayWindowPtr &ic);
	void load(const uicore::DisplayWindowPtr &ic, uicore::JsonValue buttons_node);
};
