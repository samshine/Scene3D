
#pragma once

#include <stack>
#include "Views/ScreenView/screen_view.h"

class GameMenuItem
{
public:
	GameMenuItem(std::string name, std::function<void()> click = std::function<void()>(), std::function<std::string()> value = std::function<std::string()>()) : name(name), click(click), value(value) { }

	std::string name;
	std::function<void()> click;
	std::function<std::string()> value;
};

typedef std::vector<GameMenuItem> GameMenu;

class GameMenuView : public ScreenView
{
public:
	GameMenuView();

	void update() override;

	void push_menu(GameMenu *menu);
	void pop_menu();
	void begin_edit(std::string value, std::function<void(std::string)> set_value);

	bool menu_visible() const { return !menu_stack.empty(); }

	GameMenu create_options_menu();

private:
	void on_key_down(const uicore::InputEvent &e);
	void on_mouse_up(const uicore::InputEvent &e);
	void on_mouse_move(const uicore::InputEvent &e);

	uicore::FontPtr font, font_h1;

	std::stack<GameMenu *> menu_stack;

	int current_menu_index = 0;
	bool edit_mode = false;
	std::string edit_value;
	std::function<void(std::string)> edit_set_value;
	float blink = 0.0f;
	std::vector<uicore::Rectf> item_boxes;

	GameMenu graphics_menu, audio_menu, controls_menu;
};
