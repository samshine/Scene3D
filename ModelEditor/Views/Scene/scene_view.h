
#pragma once

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <scene3d.h>

class SceneView : public clan::View
{
public:
	SceneView();
	void render_content(clan::Canvas &canvas) override;

	clan::ResourceManager &get_resources() { return resources; }
	clan::Scene &get_scene() { return scene; }

	clan::Signal<void(clan::Scene &, clan::GraphicContext &, clan::InputContext &)> sig_update_scene;

private:
	void setup_scene(clan::GraphicContext &gc);

	clan::Timer timer;

	clan::ResourceManager resources;

	clan::Texture2D scene_texture;
	clan::FrameBuffer scene_frame_buffer;

	clan::Scene scene;
};
