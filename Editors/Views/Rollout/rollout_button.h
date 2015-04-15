
#pragma once

class RolloutView;

class RolloutButton : public clan::ButtonView
{
public:
	RolloutButton(const std::string &label);

	std::shared_ptr<clan::ButtonView> button;

private:
};
