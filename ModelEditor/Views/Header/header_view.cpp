
#include "precomp.h"
#include "header_view.h"
#include "header_menu_view.h"

using namespace clan;

HeaderView::HeaderView()
{
	style()->set("flex-direction: row");
	style()->set("flex: 0 0 main-size");
	style()->set("background: linear-gradient(to right, rgb(15,50,77), rgb(95,128,146))");
	style()->set("border-bottom: 1px solid rgb(159,184,194)");

	left_buttons = std::make_shared<View>();
	left_buttons->style()->set("flex: 0 0 main-size");
	left_buttons->style()->set("flex-direction: row");
	add_subview(left_buttons);

	auto spacer = std::make_shared<View>();
	spacer->style()->set("flex: 1 1 main-size");
	add_subview(spacer);

	right_buttons = std::make_shared<View>();
	right_buttons->style()->set("flex: 0 0 main-size");
	right_buttons->style()->set("flex-direction: row");
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
	button->style()->set("flex: 0 0 main-size");
	button->style()->set("margin: auto 0");
	button->style()->set("padding: 5px 10px");
	if (!icon.empty())
	{
		if (!last)
			button->image_view()->style()->set("margin-right: 5px");
		else
			button->image_view()->style()->set("margin-left: 5px");
		button->image_view()->set_image(ImageSource::from_resource(icon));
	}
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->style()->set("font: 12px/18px 'Lato'");
	button->label()->style()->set("color: white");
	slots.connect(button->sig_pointer_release(), [=](PointerEvent &e) { click(); e.stop_propagation(); });
	if (left)
		left_buttons->add_subview(button);
	else
		right_buttons->add_subview(button);

	if (last)
		button->move_label_before_image();
}
