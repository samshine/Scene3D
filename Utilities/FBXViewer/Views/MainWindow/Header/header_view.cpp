
#include "precomp.h"
#include "header_view.h"

using namespace clan;

HeaderView::HeaderView()
{
	style.set_layout_hbox();
	style.set_flex(0.0f, 0.0f);
	style.set_background(Colorf(240, 240, 240));
	style.set_background_gradient_to_right(Colorf(15, 50, 77), Colorf(95, 128, 146));
	style.set_padding(5.0f, 2.0f);
	style.set_border(Colorf(159, 184, 194), 0.0f, 0.0f, 0.0f, 1.0f);

	create_button("Load", bind_member(this, &HeaderView::on_load));
	create_button("Save", bind_member(this, &HeaderView::on_save));
	create_button("Save As", bind_member(this, &HeaderView::on_save_as));

	auto spacer = std::make_shared<View>();
	spacer->style.set_flex(1.0f, 1.0f);
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
	FontDescription font_desc("Lato");
	font_desc.set_height(12);
	font_desc.set_line_height(1.4f * 13);

	auto button = std::make_shared<ButtonView>();
	button->style.set_flex(0.0f, 0.0f);
	button->style.set_margin(5.0f);
	//button->style.set_background(Colorf(240, 240, 240));
	//button->style.set_border(Colorf(150, 150, 150), 1.0f);
	//button->style.set_border_radius(3.0f);
	button->style.set_padding(15.0f, 5.0f);
	button->label()->set_font(Font::resource(UIThread::get_resource_canvas(), font_desc, UIThread::get_resources()));
	button->label()->set_text(StringHelp::text_to_upper(text));
	button->label()->set_text_color(Colorf(255, 255, 255));
	slots.connect(button->sig_key_press(), [=](KeyEvent &e) { click(); e.stop_propagation(); });
	add_subview(button);
}
