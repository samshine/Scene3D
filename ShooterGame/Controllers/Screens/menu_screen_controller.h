
#pragma once

#include "screen_view_controller.h"

class MenuScreenController : public ScreenViewController
{
public:
	MenuScreenController(uicore::Canvas &canvas);

	void update_desktop(uicore::Canvas &canvas, uicore::DisplayWindow &ic, const uicore::Vec2i &mouse_delta) override;

private:
	uicore::Scene scene;
	uicore::SceneObject map_object;

	float t = 0.0f;
	uicore::GameTime game_time;
};
