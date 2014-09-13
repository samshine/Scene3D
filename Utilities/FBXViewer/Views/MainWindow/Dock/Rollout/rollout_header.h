
#pragma once

#include <UICore/View/view.h>
#include <UICore/StandardViews/label_view.h>

class RolloutHeader : public uicore::View
{
public:
	RolloutHeader();
	void set_text(const std::string &text);

private:
	std::shared_ptr<uicore::LabelView> label;
};
