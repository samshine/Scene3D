
#include "precomp.h"
#include "rollout_view.h"
#include "rollout_header.h"

using namespace uicore;

RolloutView::RolloutView(const std::string &title)
{
	style()->set("flex-direction: column");

	header = std::make_shared<RolloutHeader>();
	header->set_text(title);
	add_subview(header);

	content = std::make_shared<View>();
	content->style()->set("flex-direction: column");
	content->style()->set("margin: 5px 10px");
	add_subview(content);
}

const float RolloutView::label_width = 100.0f;
