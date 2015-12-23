
#include "precomp.h"
#include "menu_screen_controller.h"
#include "game_screen_controller.h"

using namespace uicore;

MenuScreenController::MenuScreenController()
{
	slots.connect(window()->keyboard()->sig_key_down(), this, &MenuScreenController::on_key_down);
	slots.connect(window()->mouse()->sig_key_up(), this, &MenuScreenController::on_mouse_up);
	slots.connect(window()->mouse()->sig_pointer_move(), this, &MenuScreenController::on_mouse_move);

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

	create_menus();
}

void MenuScreenController::create_menus()
{
	main_menu = {
		{ "New Game", [this]() { music_player.stop(); present_controller(std::make_shared<GameScreenController>("localhost", "5004", true)); } },
		{ "Join Game", [this]() { push_menu(&join_menu); } },
		{ "Host Game", [this]() { push_menu(&host_menu); } },
		{ "Options", [this]() { push_menu(&options_menu); } },
		{ "Quit", [this]() { exit_game(); } }
	};

	join_menu = {
		{ "Join Game", [this]() { music_player.stop(); present_controller(std::make_shared<GameScreenController>(server, port, false)); } },
		{ "Player Name:", [this]() { begin_edit(&player_name); }, [this]() { return player_name; } },
		{ "Server:", [this]() { begin_edit(&server); }, [this]() { return server; } },
		{ "Port:", [this]() { begin_edit(&port); }, [this]() { return port; } },
		{ "Back", [this]() { pop_menu(); } }
	};

	host_menu = {
		{ "Create Game", [this]() { music_player.stop(); present_controller(std::make_shared<GameScreenController>(server, port, false)); } },
		{ "Player Name:", [this]() { begin_edit(&player_name); }, [this]() { return player_name; } },
		{ "Port:", [this]() { begin_edit(&port); }, [this]() { return port; } },
		{ "Back", [this]() { pop_menu(); } }
	};

	options_menu = {
		{ "Graphics", [this]() { push_menu(&graphics_menu); } },
		{ "Audio", [this]() { push_menu(&audio_menu); } },
		{ "Controls", [this]() { push_menu(&controls_menu); } },
		{ "Back", [this]() { pop_menu(); } }
	};

	graphics_menu = {
		{ "Quality:", [this]() {}, [this]() { return "High"; } },
		{ "Gamma:", [this]() {}, [this]() { return "2.2"; } },
		{ "V-Sync:", [this]() {}, [this]() { return "On"; } },
		{ "Back", [this]() { pop_menu(); } }
	};

	audio_menu = {
		{ "Master Volume:", [this]() {}, [this]() { return "100"; } },
		{ "Music Volume:", [this]() {}, [this]() { return "75"; } },
		{ "SFX Volume:", [this]() {}, [this]() { return "100"; } },
		{ "Back", [this]() { pop_menu(); } }
	};

	controls_menu = {
		{ "Back", [this]() { pop_menu(); } }
	};

	menu_stack.push(&main_menu);
}

void MenuScreenController::push_menu(GameMenu *menu)
{
	menu_stack.push(menu);
	item_boxes.clear();
	current_menu_index = 0;
}

void MenuScreenController::pop_menu()
{
	menu_stack.pop();
	item_boxes.clear();
	current_menu_index = 0;
}

void MenuScreenController::begin_edit(std::string *value)
{
	edit_mode = true;
	edit_value = value;
}

void MenuScreenController::on_key_down(const uicore::InputEvent &e)
{
	if (edit_mode)
	{
		if (e.id == keycode_enter)
			edit_mode = false;
		else if (e.id == keycode_backspace && !edit_value->empty())
			edit_value->pop_back();
		else if (!e.str.empty())
			edit_value->append(e.str);
	}
	else
	{
		if (e.id == keycode_up)
		{
			current_menu_index = std::max(current_menu_index - 1, 0);
		}
		else if (e.id == keycode_down)
		{
			current_menu_index = std::min(current_menu_index + 1, (int)menu_stack.top()->size() - 1);
		}
		else if (e.id == keycode_enter)
		{
			menu_stack.top()->at(current_menu_index).click();
		}
	}
}

void MenuScreenController::on_mouse_up(const uicore::InputEvent &e)
{
	if (edit_mode)
		return;

	if (e.id == mouse_left)
	{
		int i = 0;
		for (auto &item_box : item_boxes)
		{
			if (item_box.contains(window()->mouse()->position()))
			{
				current_menu_index = i;
				menu_stack.top()->at(current_menu_index).click();
				break;
			}
			i++;
		}
	}
}

void MenuScreenController::on_mouse_move(const uicore::InputEvent &e)
{
	if (edit_mode)
		return;

	int i = 0;
	for (auto &item_box : item_boxes)
	{
		if (item_box.contains(window()->mouse()->position()))
		{
			current_menu_index = i;
			break;
		}
		i++;
	}
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

	canvas()->begin();

	Path::rect(canvas()->size())->fill(canvas(), Brush::solid(Colorf(0.0f, 0.0f, 0.0f, 0.2f)));

	const auto &menu = *menu_stack.top();

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

	blink = std::fmod(blink + game_time().get_time_elapsed() * 2.0f, 2.0f);

	float y = (canvas()->size().height - 5.0f * line_height) * 0.5f;

	int i = 0;
	item_boxes.clear();
	for (const auto &item : menu)
	{
		std::string edit_text;

		float advance_width = font->measure_text(canvas(), item.name).advance.width;
		float edit_advance_width = 0.0f;
		if (item.value)
		{
			edit_text = item.value();

			if (i == current_menu_index && edit_mode)
				edit_text += "_";

			edit_advance_width = font->measure_text(canvas(), edit_text).advance.width;
			advance_width = std::max(400.0f, advance_width + edit_advance_width + 30.0f);

			if (i == current_menu_index && edit_mode && blink >= 1.0f)
				edit_text.pop_back();
		}

		float x0 = (canvas()->size().width - advance_width) * 0.5f;
		float x1 = (canvas()->size().width + advance_width) * 0.5f - edit_advance_width;

		font->draw_text(canvas(), x0 + 2.0f, y + 2.0f, item.name, Colorf::black);
		font->draw_text(canvas(), x0, y, item.name, i == current_menu_index && !edit_mode ? Colorf::floralwhite : Colorf::lightsteelblue);

		font->draw_text(canvas(), x1 + 2.0f, y + 2.0f, edit_text, Colorf::black);
		font->draw_text(canvas(), x1, y, edit_text, i == current_menu_index && edit_mode ? Colorf::floralwhite : Colorf::lightsteelblue);

		item_boxes.push_back(Rectf::xywh(x0, y - font_metrics.ascent(), advance_width, font_metrics.height()));

		y += line_height;
		i++;
	}

	if (fade_time < 1.0f)
		Path::rect(canvas()->size())->fill(canvas(), Brush::solid(Colorf(0.0f, 0.0f, 0.0f, clamp(1.0f - fade_time, 0.0f, 1.0f))));

	canvas()->end();
}
