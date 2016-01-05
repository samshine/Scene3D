
#include "precomp.h"
#include "game_menu_view.h"
#include "Model/Settings/settings.h"

using namespace uicore;

GameMenuView::GameMenuView()
{
	slots.connect(window()->keyboard()->sig_key_down(), this, &GameMenuView::on_key_down);
	slots.connect(window()->mouse()->sig_key_up(), this, &GameMenuView::on_mouse_up);
	slots.connect(window()->mouse()->sig_pointer_move(), this, &GameMenuView::on_mouse_move);

	FontDescription font_desc;
	font_desc.set_height(30.0f);
	font_desc.set_line_height(40.0f);
	font = Font::create(font_desc, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	FontDescription font_desc_h1;
	font_desc_h1.set_height(50.0f);
	font_desc_h1.set_line_height(100.0f);
	font_h1 = Font::create(font_desc_h1, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");
}

GameMenu GameMenuView::create_options_menu()
{
	auto mouse_speed_x = []() -> std::string { return Text::to_string(Settings::mouse_speed_x(), 1); };
	auto set_mouse_speed_x = [](std::string value) { try { Settings::set_mouse_speed_x(Text::parse_float(value)); } catch (...) { } };

	auto mouse_speed_y = []() -> std::string { return Text::to_string(Settings::mouse_speed_y(), 1); };
	auto set_mouse_speed_y = [](std::string value) { try { Settings::set_mouse_speed_y(Text::parse_float(value)); } catch (...) { } };

	GameMenu options_menu = {
		{ "Graphics", [=]() { push_menu(&graphics_menu); } },
		{ "Audio", [=]() { push_menu(&audio_menu); } },
		{ "Controls", [=]() { push_menu(&controls_menu); } },
		{ "Back", [=]() { pop_menu(); } }
	};

	graphics_menu = {
		{ "Quality:", [=]() {}, [=]() { return "High"; } },
		{ "Gamma:", [=]() {}, [=]() { return "2.2"; } },
		{ "V-Sync:", [=]() {}, [=]() { return "On"; } },
		{ "Back", [=]() { pop_menu(); } }
	};

	audio_menu = {
		{ "Master Volume:", [=]() {}, [=]() { return "100"; } },
		{ "Music Volume:", [=]() {}, [=]() { return "75"; } },
		{ "SFX Volume:", [=]() {}, [=]() { return "100"; } },
		{ "Back", [=]() { pop_menu(); } }
	};

	controls_menu = {
		{ "Mouse speed X:", [=]() { begin_edit(mouse_speed_x(), set_mouse_speed_x); }, mouse_speed_x },
		{ "Mouse speed Y:", [=]() { begin_edit(mouse_speed_y(), set_mouse_speed_y); }, mouse_speed_y },
		{ "Back", [=]() { pop_menu(); } }
	};

	return options_menu;
}

void GameMenuView::push_menu(GameMenu *menu)
{
	menu_stack.push(menu);
	item_boxes.clear();
	current_menu_index = 0;
}

void GameMenuView::pop_menu()
{
	menu_stack.pop();
	item_boxes.clear();
	current_menu_index = 0;
}

void GameMenuView::begin_edit(std::string value, std::function<void(std::string)> set_value)
{
	edit_mode = true;
	edit_value = value;
	edit_set_value = set_value;
}

void GameMenuView::on_key_down(const uicore::InputEvent &e)
{
	if (edit_mode)
	{
		if (e.id == keycode_enter)
		{
			edit_mode = false;
			if (edit_set_value)
				edit_set_value(edit_value);
			edit_set_value = std::function<void(std::string)>();
		}
		else if (e.id == keycode_backspace && !edit_value.empty())
		{
			edit_value.pop_back();
		}
		else if (!e.str.empty())
		{
			edit_value.append(e.str);
		}
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

void GameMenuView::on_mouse_up(const uicore::InputEvent &e)
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

void GameMenuView::on_mouse_move(const uicore::InputEvent &e)
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

void GameMenuView::update()
{
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

	blink = std::fmod(blink + game_time().time_elapsed() * 2.0f, 2.0f);

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
			if (i == current_menu_index && edit_mode)
			{
				edit_text = edit_value + "_";
			}
			else
			{
				edit_text = item.value();
			}

			edit_advance_width = font->measure_text(canvas(), edit_text).advance.width;
			advance_width = std::max(500.0f, advance_width + edit_advance_width + 30.0f);

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
}
