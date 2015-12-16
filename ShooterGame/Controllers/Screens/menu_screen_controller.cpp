
#include "precomp.h"
#include "menu_screen_controller.h"
#include "game_screen_controller.h"

using namespace uicore;

MenuScreenController::MenuScreenController()
{
	FontDescription font_desc;
	font_desc.set_height(30.0f);
	font_desc.set_line_height(40.0f);
	font = Font::create(font_desc, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	scene = Scene::create(scene_engine());
	scene_camera = SceneCamera::create(scene);
	scene_camera->set_position(Vec3f(0.0f, 1.8f, -3.0f));
	//scene_camera->set_orientation(Quaternionf(0.0f, 180.0f, 0.0f, angle_degrees, order_YXZ));

	scene->show_skybox_stars(false);
	std::vector<Colorf> gradient;
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 5 / 10, 240 * 5 / 10, 243 * 5 / 10));
	gradient.push_back(Colorf(236 * 6 / 10, 240 * 6 / 10, 243 * 6 / 10));
	gradient.push_back(Colorf(236 * 7 / 10, 240 * 7 / 10, 243 * 7 / 10));
	gradient.push_back(Colorf(236 * 8 / 10, 240 * 8 / 10, 243 * 8 / 10));
	for (auto &g : gradient)
	{
		g.r = std::pow(g.r, 2.2f);
		g.g = std::pow(g.g, 2.2f);
		g.b = std::pow(g.b, 2.2f);
	}
	scene->set_skybox_gradient(gradient);

	auto model = SceneModel::create(scene, "Models/Liandri/liandri.cmodel");
	map_object = SceneObject::create(scene, model);
}

void MenuScreenController::update()
{
	t = std::fmod(t + game_time().get_time_elapsed() * 0.01f, 2.0f);

	float t2 = t > 1.0f ? 2.0f - t : t;
	scene_camera->set_position(Vec3f(-12.0f, 2.5f + 1.8f, -13.0f - 3.0f * t2));

	scene_viewport()->set_camera(scene_camera);
	scene_viewport()->update(gc(), game_time().get_time_elapsed());
	scene_viewport()->render(gc());

	bool up_pressed = window()->keyboard()->keycode(keycode_up);
	bool down_pressed = window()->keyboard()->keycode(keycode_down);
	bool mouse_pressed = window()->mouse()->keycode(mouse_left);
	bool enter_pressed = window()->keyboard()->keycode(keycode_enter);
	if (up_pressed && !up_was_pressed)
		current_menu_index = std::max(current_menu_index - 1, 0);
	else if (down_pressed && !down_was_pressed)
		current_menu_index = std::min(current_menu_index + 1, 4);

	canvas()->begin();

	auto font_metrics = font->font_metrics(canvas());
	float line_height = font_metrics.line_height();
	float y = (canvas()->size().height - 5.0f * line_height) * 0.5f;
	int i = 0;
	for (const auto &str : { "New Game", "Join Game", "Host Game", "Options", "Quit" })
	{
		float advance_width = font->measure_text(canvas(), str).advance.width;
		float x = (canvas()->size().width - advance_width) * 0.5f;
		font->draw_text(canvas(), x, y, str, i == current_menu_index ? Colorf::white : Colorf::gray50);

		if (Rectf::xywh(x, y - font_metrics.ascent(), advance_width, font_metrics.height()).contains(window()->mouse()->position()))
		{
			if (!mouse_pressed && mouse_was_pressed)
				enter_pressed = true;
			current_menu_index = i;
		}

		y += line_height;
		i++;
	}

	canvas()->end();

	up_was_pressed = up_pressed;
	down_was_pressed = down_pressed;
	mouse_was_pressed = mouse_pressed;

	if (enter_pressed)
	{
		switch (current_menu_index)
		{
		case 0:
			present_controller(std::make_shared<GameScreenController>());
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			exit_game();
			break;
		}
	}
}
