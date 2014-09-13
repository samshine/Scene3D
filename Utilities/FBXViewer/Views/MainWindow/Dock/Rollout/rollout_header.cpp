
#include "rollout_header.h"

using namespace uicore;

RolloutHeader::RolloutHeader()
{
	style.set_layout_block();
	style.set_margin(5.0f);
	style.set_padding(10.0f, 2.0f);
	style.set_background(Colorf::rgb8(230, 230, 230));

	label = std::make_shared<LabelView>();
	label->set_font(FontDescription("Segoe UI", 13.0f, 1.4f));
	add_subview(label);
}

void RolloutHeader::set_text(const std::string &text)
{
	label->set_text(text);
}
