
#include "precomp.h"
#include "rollout_header.h"

using namespace clan;

RolloutHeader::RolloutHeader()
{
	style.set_layout_block();
	style.set_margin(5.0f);
	style.set_padding(10.0f, 2.0f);
	style.set_background(Colorf(230, 230, 230));

	FontDescription font_desc("Segoe UI");
	font_desc.set_height(13);
	font_desc.set_line_height(1.4f * 13);

	label = std::make_shared<LabelView>();
	label->set_font(font_desc);
	add_subview(label);
}

void RolloutHeader::set_text(const std::string &text)
{
	label->set_text(text);
}
