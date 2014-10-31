
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

	create_button("Load", bind_member(this, &HeaderView::on_load));
	create_button("Save", bind_member(this, &HeaderView::on_save));
	create_button("Save As", bind_member(this, &HeaderView::on_save_as));

	auto spacer = std::make_shared<View>();
	spacer->box_style.set_flex(1.0f, 1.0f);
	add_subview(spacer);

	create_button("Animations", bind_member(this, &HeaderView::on_options));
}

void HeaderView::on_load()
{
}

void HeaderView::on_save()
{
}

void HeaderView::on_save_as()
{
}

void HeaderView::on_options()
{
}

void HeaderView::create_button(const std::string &text, std::function<void()> click)
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
	slots.connect(button->sig_key_press(), [=](KeyEvent &e) { click(); e.stop_propagation(); });
	add_subview(button);
}
