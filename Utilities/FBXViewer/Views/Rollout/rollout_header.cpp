
#include "precomp.h"
#include "rollout_header.h"

using namespace clan;

RolloutHeader::RolloutHeader()
{
	box_style.set_layout_block();
	//box_style.set_margin(5.0f);
	box_style.set_padding(10.0f, 2.0f);
	box_style.set_background(Colorf(6, 31, 62));

	label = std::make_shared<LabelView>();
	label->text_style().set_font("Lato", 12, 1.4f * 13);
	label->text_style().set_color(Colorf(255, 255, 255));
	add_subview(label);
}

void RolloutHeader::set_text(const std::string &text)
{
	label->set_text(text);
}
