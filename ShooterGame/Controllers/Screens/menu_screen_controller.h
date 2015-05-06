
#pragma once

#include "screen_view_controller.h"

class MenuScreenController : public ScreenViewController
{
public:
	MenuScreenController(clan::Canvas &canvas);

	void update_desktop(clan::Canvas &canvas, clan::InputContext &ic, const clan::Vec2i &mouse_delta) override;

private:
	clan::Scene scene;
	clan::SceneObject map_object;

	float t = 0.0f;
	clan::GameTime game_time;
};
