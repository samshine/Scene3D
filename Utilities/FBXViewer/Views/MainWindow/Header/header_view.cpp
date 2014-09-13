
#include "header_view.h"
#include <UICore/Events/key_event.h>

using namespace uicore;

HeaderView::HeaderView()
{
	style.set_layout_hbox();
	style.set_flex(0.0f, 0.0f);
	style.set_background(Colorf::rgb8(240, 240, 240));
	style.set_padding(11.0f);

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
	auto button = std::make_shared<ButtonView>();
	button->style.set_flex(0.0f, 0.0f);
	button->style.set_margin(5.0f);
	button->style.set_background(Colorf::rgb8(240, 240, 240));
	button->style.set_border(Colorf::rgb8(150, 150, 150), 1.0f);
	button->style.set_border_radius(3.0f);
	button->style.set_padding(15.0f, 5.0f);
	button->label()->set_font(FontDescription("Segoe UI", 13.0f, 1.4f));
	button->label()->set_text(text);
	button->label()->set_text_color(Colorf::rgb8(0, 0, 0));
	slots.connect(button->sig_key_press(), [=](KeyEvent &e) { click(); e.stop_propagation(); });
	add_subview(button);
}
