
#pragma once

class RolloutView;

class RolloutButton : public uicore::ButtonBaseView
{
public:
	RolloutButton(const std::string &label);

	std::shared_ptr<uicore::ButtonBaseView> button;

private:
};
