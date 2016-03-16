
#pragma once

class RolloutView;

class RolloutPositionProperty : public uicore::View
{
public:
	RolloutPositionProperty(const std::string &label);

	std::shared_ptr<uicore::LabelBaseView> label;
	std::shared_ptr<uicore::LabelBaseView> label_x;
	std::shared_ptr<uicore::LabelBaseView> label_y;
	std::shared_ptr<uicore::LabelBaseView> label_z;
	std::shared_ptr<uicore::TextFieldBaseView> input_x;
	std::shared_ptr<uicore::TextFieldBaseView> input_y;
	std::shared_ptr<uicore::TextFieldBaseView> input_z;

	uicore::Signal<void()> &sig_value_changed() { return _sig_value_changed; }

private:
	uicore::Signal<void()> _sig_value_changed;
};
