
#include "precomp.h"
#include "header_view.h"
#include "header_menu_view.h"

using namespace clan;

HeaderView::HeaderView()
{
	box_style.set_layout_hbox();
	box_style.set_flex(0.0f, 0.0f);
	box_style.set_background(Colorf(240, 240, 240));
	box_style.set_background_gradient_to_right(Colorf(15, 50, 77), Colorf(95, 128, 146));
	box_style.set_border(Colorf(159, 184, 194), 0.0f, 0.0f, 0.0f, 1.0f);

	left_buttons = std::make_shared<View>();
	left_buttons->box_style.set_flex(0.0f, 0.0f);
	left_buttons->box_style.set_layout_hbox();
	add_subview(left_buttons);

	auto spacer = std::make_shared<View>();
	spacer->box_style.set_flex(1.0f, 1.0f);
	add_subview(spacer);

	right_buttons = std::make_shared<View>();
	right_buttons->box_style.set_flex(0.0f, 0.0f);
	right_buttons->box_style.set_layout_hbox();
	add_subview(right_buttons);
}

void HeaderView::add_left_button(const std::string &text, std::function<void()> click)
{
	create_button(text, std::string(), click, true);
}

void HeaderView::add_left_button(const std::string &text, const std::string &icon, std::function<void()> click)
{
	create_button(text, icon, click, true);
}

void HeaderView::add_right_button(const std::string &text, std::function<void()> click)
{
	create_button(text, std::string(), click, false);
}

void HeaderView::add_right_button(const std::string &text, const std::string &icon, std::function<void()> click, bool last)
{
	create_button(text, icon, click, false, last);
}

std::shared_ptr<HeaderMenuView> HeaderView::add_left_menu(const std::string &text, const std::string &icon)
{
	auto menu = std::make_shared<HeaderMenuView>(text, icon, false);
	left_buttons->add_subview(menu);
	return menu;
}

std::shared_ptr<HeaderMenuView> HeaderView::add_right_menu(const std::string &text, const std::string &icon, bool last)
{
	auto menu = std::make_shared<HeaderMenuView>(text, icon, last);
	right_buttons->add_subview(menu);
	return menu;
}

void HeaderView::create_button(const std::string &text, const std::string &icon, std::function<void()> click, bool left, bool last)
{
	auto button = std::make_shared<ButtonView>();
	button->box_style.set_flex(0.0f, 0.0f);
	//button->box_style.set_margin(5.0f);
	button->box_style.set_margin_top_auto();
	button->box_style.set_margin_bottom_auto();
	//button->box_style.set_background(Colorf(240, 240, 240));
	//button->box_style.set_border(Colorf(150, 150, 150), 1.0f);
	//button->box_style.set_border_radius(3.0f);
	button->box_style.set_padding(10.0f, 5.0f);
	if (!icon.empty())
	{
		if (!last)
			button->image_view()->box_style.set_margin(0.0f, 0.0, 5.0f, 0.0f);
		else
			button->image_view()->box_style.set_margin(5.0f, 0.0, 0.0f, 0.0f);
		button->image_view()->set_image(ImageSource::from_resource(icon));
	}
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->text_style().set_font("Lato", 12, 1.4f * 13);
	button->label()->text_style().set_color(Colorf(255, 255, 255));
	slots.connect(button->sig_pointer_release(EventUIPhase::bubbling), [=](PointerEvent &e) { click(); e.stop_propagation(); });
	if (left)
		left_buttons->add_subview(button);
	else
		right_buttons->add_subview(button);

	if (last)
		button->move_label_before_image();
}
