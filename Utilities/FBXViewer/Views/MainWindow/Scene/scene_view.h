
#pragma once

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/scene3d.h>
#include <UICore/View/view.h>
#include <UICore/StandardViews/button_view.h>
#include <UICore/Animation/timer.h>

class SceneView : public uicore::View
{
public:
	SceneView();
	void render_content(uicore::Canvas &canvas) override;

private:
	void setup_scene(clan::GraphicContext &gc);
	void update_model(clan::GraphicContext &gc);

	std::string current_animation = "default";

	uicore::Timer timer;

	clan::ResourceManager resources;

	clan::Scene scene;

	clan::SceneLight light1;
	clan::SceneLight light2;
	clan::SceneModel model1;
	clan::SceneObject object1;
	clan::SceneCamera camera;

	clan::GameTime gametime;

	float dir = 180.0f;
	float up = 0.0f;
	float turn_speed = 90.0f;

	bool model_data_updated = true;

	int keys_down = 0;
};
