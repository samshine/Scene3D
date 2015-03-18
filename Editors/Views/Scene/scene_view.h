
#pragma once

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <scene3d.h>
#include "mouse_movement.h"

class SceneView : public clan::View
{
public:
	SceneView();
	void render_content(clan::Canvas &canvas) override;

	clan::ResourceManager &get_resources() { return resources; }
	clan::Scene &get_scene() { return scene; }

	clan::Signal<void(clan::Scene &, clan::GraphicContext &, clan::InputContext &, const clan::Vec2i &)> sig_update_scene;

private:
	void pointer_press(clan::PointerEvent &e);
	void pointer_release(clan::PointerEvent &e);
	void pointer_move(clan::PointerEvent &e);

	void setup_scene(clan::GraphicContext &gc);

	clan::Timer timer;
	MouseMovement mouse_movement;
	clan::Pointf mouse_down_pos;
	bool mouse_down = false;
	clan::Point last_mouse_movement;

	clan::ResourceManager resources;

	clan::Texture2D scene_texture;
	clan::FrameBuffer scene_frame_buffer;

	clan::Scene scene;
};
