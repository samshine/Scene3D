
#include "precomp.h"
#include "header_view.h"

using namespace clan;

HeaderView::HeaderView()
{
	box_style.set_layout_hbox();
	box_style.set_flex(0.0f, 0.0f);
	box_style.set_background(Colorf(240, 240, 240));
	box_style.set_background_gradient_to_right(Colorf(15, 50, 77), Colorf(95, 128, 146));
	box_style.set_padding(5.0f, 2.0f);
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
	create_button(text, click, true);
}

void HeaderView::add_right_button(const std::string &text, std::function<void()> click)
{
	create_button(text, click, false);
}

void HeaderView::create_button(const std::string &text, std::function<void()> click, bool left)
{
	auto button = std::make_shared<ButtonView>();
	button->box_style.set_flex(0.0f, 0.0f);
	button->box_style.set_margin(5.0f);
	//button->box_style.set_background(Colorf(240, 240, 240));
	//button->box_style.set_border(Colorf(150, 150, 150), 1.0f);
	//button->box_style.set_border_radius(3.0f);
	button->box_style.set_padding(15.0f, 5.0f);
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->text_style().set_font("Lato", 12, 1.4f * 13);
	button->label()->text_style().set_color(Colorf(255, 255, 255));
	slots.connect(button->sig_pointer_release(EventUIPhase::bubbling), [=](PointerEvent &e) { click(); e.stop_propagation(); });
	if (left)
		left_buttons->add_subview(button);
	else
		right_buttons->add_subview(button);
}
