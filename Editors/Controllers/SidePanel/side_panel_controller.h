
#pragma once

class SidePanelController : public clan::ViewController
{
public:
	SidePanelController();

	std::shared_ptr<clan::View> content_view() const;
};
