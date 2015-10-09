
#pragma once

#include "screen_view_controller.h"

class MenuScreenController : public ScreenViewController
{
public:
	MenuScreenController(uicore::Canvas &canvas);

	void update_desktop(uicore::Canvas &canvas, const uicore::DisplayWindowPtr &ic, const uicore::Vec2i &mouse_delta) override;

private:
	Scene scene;
	SceneObject map_object;

	float t = 0.0f;
	uicore::GameTime game_time;
};
