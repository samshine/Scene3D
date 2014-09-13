
#include "rollout_view.h"
#include "rollout_header.h"

RolloutView::RolloutView(const std::string &title)
{
	style.set_layout_block();

	header = std::make_shared<RolloutHeader>();
	header->set_text(title);
	add_subview(header);

	content = std::make_shared<View>();
	content->style.set_margin(15.0f, 5.0f);
	add_subview(content);
}

const float RolloutView::label_width = 100.0f;
