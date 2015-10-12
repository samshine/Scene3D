
#pragma once

#include "screen_view_controller.h"
#include "Model/game.h"

class GameScreenController : public ScreenViewController
{
public:
	GameScreenController(const uicore::CanvasPtr &canvas);
	bool cursor_hidden() override { return true; }
	void update_desktop(const uicore::CanvasPtr &canvas, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta) override;

private:
	std::unique_ptr<Game> client_game;
	std::unique_ptr<Game> server_game;
	bool desktop_exception_flag = false;
};

/*
#include "screen_view_controller.h"
#include "character_controller.h"

class GameInputButton
{
public:
	GameInputButton() { }
	GameInputButton(int keycode) : keycode(keycode) { }
	bool pressed() const { return is_down; }
	bool clicked() const { return is_click; }

	void update(const uicore::DisplayWindowPtr &ic)
	{
		if (ic.get_keyboard().get_keycode(keycode))
		{
			is_click = !is_down;
			is_down = true;
		}
		else
		{
			is_click = false;
			is_down = false;
		}
	}

private:
	bool is_down = false;
	bool is_click = false;
	int keycode = 0;

	friend class GameInput;
};

class GameInput
{
public:
	const GameInputButton &button(int keycode)
	{
		auto &button = keys[keycode];
		if (button.keycode == 0)
			button = GameInputButton(keycode);
		return button;
	}

	void update_buttons(uicore::DisplayWindow &ic)
	{
		for (auto &it : keys)
			it.second.update(ic);
	}

private:
	std::map<int, GameInputButton> keys;
};

class GameScreenController : public ScreenViewController
{
public:
	GameScreenController(const uicore::CanvasPtr &canvas);
	bool cursor_hidden() override { return true; }
	void update_desktop(const uicore::CanvasPtr &canvas, uicore::DisplayWindow &ic, const uicore::Vec2i &mouse_delta) override;

private:
	void update_game(uicore::DisplayWindow &ic);
	void update_camera();
	void update_look_dir(const uicore::Vec2i &mouse_delta);
	void update_character_controller();

	uicore::GameTime game_time;

	Scene scene;
	uicore::Physics3DWorld collision_world;
	CharacterController character_controller;

	SceneObject map_object;
	uicore::Physics3DObject map_collision;

	SceneObject model_object;
	std::string last_anim = "default";

	GameInput game_input;

	float dodge_cooldown = 0;
	float double_tap_left_elapsed = 1;
	float double_tap_right_elapsed = 1;
	float double_tap_up_elapsed = 1;
	float double_tap_down_elapsed = 1;
	bool was_down_left = false;
	bool was_down_right = false;
	bool was_down_up = false;
	bool was_down_down = false;
};
*/
