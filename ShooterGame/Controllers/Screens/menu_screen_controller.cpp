
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

	FontDescription font_desc_h1;
	font_desc_h1.set_height(50.0f);
	font_desc_h1.set_line_height(100.0f);
	font_h1 = Font::create(font_desc_h1, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	scene = Scene::create(scene_engine());
	scene_camera = SceneCamera::create(scene);
	scene_camera->set_position(Vec3f(0.0f, 1.8f, -3.0f));
	scene_camera->set_orientation(Quaternionf(10.0f, 22.0f, 4.0f, angle_degrees, order_YXZ));

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

	auto map_data = MapData::load("Resources/Assets/Levels/Liandri/liandri2.cmap");
	for (const auto &item : map_data->objects)
	{
		if (item.type != "Static" && item.type != "Level")
			continue;

		Vec3f position = item.position;
		Vec3f scale = Vec3f(item.scale);
		Vec3f rotate(item.dir, item.up, item.tilt);
		std::string model_name = item.mesh;
		std::string animation_name = item.animation;
		scene_objects.push_back(SceneObject::create(scene, SceneModel::create(scene, model_name), position, Quaternionf(rotate.y, rotate.x, rotate.z, angle_degrees, order_YXZ), scale));
		scene_objects.back()->play_animation(animation_name, true);
	}

	music_player.play("Resources/Assets/Music/menu.ogg", true);
}

void MenuScreenController::update()
{
	fade_time = std::min(fade_time + game_time().get_time_elapsed() * 2.0f, 1.0f);

	t = std::fmod(t + game_time().get_time_elapsed() * 0.01f, 2.0f);

	float t2 = t > 1.0f ? 2.0f - t : t;
	scene_camera->set_position(Vec3f(-12.0f, 2.5f + 1.8f, -12.0f - 3.0f * t2));

	scene_viewport()->set_camera(scene_camera);
	for (const auto &item : scene_objects)
		item->update(game_time().get_time_elapsed());
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

	Path::rect(canvas()->size())->fill(canvas(), Brush::solid(Colorf(0.0f, 0.0f, 0.0f, 0.2f)));

	auto font_metrics_h1 = font_h1->font_metrics(canvas());
	auto font_metrics = font->font_metrics(canvas());
	float line_height_h1 = font_metrics_h1.line_height();
	float line_height = font_metrics.line_height();

	{
		float y = canvas()->size().height / 4.0f;
		std::string str = "Shooter Game!";
		float advance_width = font_h1->measure_text(canvas(), str).advance.width;
		float x = (canvas()->size().width - advance_width) * 0.5f;
		font_h1->draw_text(canvas(), x + 2.0f, y + 2.0f, str, Colorf::black);
		font_h1->draw_text(canvas(), x, y, str, Colorf::palegoldenrod);
	}

	float y = (canvas()->size().height - 5.0f * line_height) * 0.5f;

	int i = 0;
	for (const auto &str : { "New Game", "Join Game", "Host Game", "Options", "Quit" })
	{
		float advance_width = font->measure_text(canvas(), str).advance.width;
		float x = (canvas()->size().width - advance_width) * 0.5f;
		font->draw_text(canvas(), x + 2.0f, y + 2.0f, str, Colorf::black);
		font->draw_text(canvas(), x, y, str, i == current_menu_index ? Colorf::floralwhite : Colorf::lightsteelblue);

		if (Rectf::xywh(x, y - font_metrics.ascent(), advance_width, font_metrics.height()).contains(window()->mouse()->position()))
		{
			if (!mouse_pressed && mouse_was_pressed)
				enter_pressed = true;
			current_menu_index = i;
		}

		y += line_height;
		i++;
	}

	if (fade_time < 1.0f)
		Path::rect(canvas()->size())->fill(canvas(), Brush::solid(Colorf(0.0f, 0.0f, 0.0f, clamp(1.0f - fade_time, 0.0f, 1.0f))));

	canvas()->end();

	up_was_pressed = up_pressed;
	down_was_pressed = down_pressed;
	mouse_was_pressed = mouse_pressed;

	if (enter_pressed)
	{
		switch (current_menu_index)
		{
		case 0:
			music_player.stop();
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
