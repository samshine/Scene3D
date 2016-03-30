
#include "precomp.h"
#include "rollout_header.h"

using namespace uicore;

RolloutHeader::RolloutHeader()
{
	style()->set(R"(
		flex-direction: column;
		padding: 2px 10px;
		background: rgb(58,58,58); /*rgb(6,31,62)*/
		)");

	label = std::make_shared<LabelBaseView>();
	label->style()->set("font: 12px/18px 'Lato'");
	label->style()->set("color: rgb(230,230,230)");
	add_child(label);
}

void RolloutHeader::set_text(const std::string &text)
{
	label->set_text(text);
}
