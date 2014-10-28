
#include "precomp.h"
#include "rollout_header.h"

using namespace clan;

RolloutHeader::RolloutHeader()
{
	style.set_layout_block();
	//style.set_margin(5.0f);
	style.set_padding(10.0f, 2.0f);
	style.set_background(Colorf(6, 31, 62));

	FontDescription font_desc("Lato");
	font_desc.set_height(12);
	font_desc.set_line_height(1.4f * 13);

	label = std::make_shared<LabelView>();
	label->set_font(Font::resource(UIThread::get_resource_canvas(), font_desc, UIThread::get_resources()));
	label->set_text_color(Colorf(255, 255, 255));
	add_subview(label);
}

void RolloutHeader::set_text(const std::string &text)
{
	label->set_text(text);
}
