
#pragma once

class RolloutView;

class RolloutButton : public uicore::ButtonView
{
public:
	RolloutButton(const std::string &label);

	std::shared_ptr<uicore::ButtonView> button;

private:
};
