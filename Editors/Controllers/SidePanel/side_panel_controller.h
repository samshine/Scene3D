
#pragma once

class SidePanelController : public uicore::ViewController
{
public:
	SidePanelController();

	std::shared_ptr<uicore::View> content_view() const;
};
