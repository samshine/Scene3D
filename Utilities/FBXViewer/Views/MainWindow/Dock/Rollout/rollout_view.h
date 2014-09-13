
#pragma once

#include <UICore/View/view.h>

class RolloutHeader;

class RolloutView : public uicore::View
{
public:
	RolloutView(const std::string &title);

	std::shared_ptr<RolloutHeader> header;
	std::shared_ptr<View> content;

	static const float label_width;

private:
};
