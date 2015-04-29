
#pragma once

#include "screen_view_controller.h"

class MenuScreenController : public ScreenViewController
{
public:
	MenuScreenController(clan::Canvas &canvas);

	void update_desktop(clan::Canvas &canvas) override;

private:
	clan::Scene scene;
	clan::SceneObject map_object;
};
