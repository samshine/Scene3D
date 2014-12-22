
#pragma once

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <scene3d.h>

class SceneView : public clan::View
{
public:
	SceneView();
	void render_content(clan::Canvas &canvas) override;

	void set_model_data(std::shared_ptr<clan::ModelData> model_data);

	void play_animation(const std::string &name, bool instant);
	void play_transition(const std::string &anim1, const std::string &anim2, bool instant);
	std::string get_animation() const;

	clan::Signal<void(clan::Scene &, clan::GraphicContext &, clan::InputContext &)> sig_update_scene;

private:
	void setup_scene(clan::GraphicContext &gc);
	void update_model(clan::GraphicContext &gc);

	std::shared_ptr<clan::ModelData> model_data;
	std::string current_animation = "default";

	clan::Timer timer;

	clan::ResourceManager resources;

	clan::Texture2D scene_texture;
	clan::FrameBuffer scene_frame_buffer;

	clan::Scene scene;

	clan::SceneLight light1;
	clan::SceneLight light2;
	clan::SceneModel model1;
	clan::SceneModel model2;
	clan::SceneObject object1;
	clan::SceneObject attach1;
	clan::SceneCamera camera;

	clan::GameTime gametime;

	float dir = 0.0f;
	float up = 0.0f;
	float turn_speed = 90.0f;

	bool model_data_updated = true;

};
