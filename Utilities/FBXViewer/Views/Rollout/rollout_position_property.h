
#pragma once

class RolloutView;

class RolloutPositionProperty : public clan::View
{
public:
	RolloutPositionProperty(const std::string &label);

	std::shared_ptr<clan::LabelView> label;
	std::shared_ptr<clan::LabelView> label_x;
	std::shared_ptr<clan::LabelView> label_y;
	std::shared_ptr<clan::LabelView> label_z;
	std::shared_ptr<clan::TextFieldView> input_x;
	std::shared_ptr<clan::TextFieldView> input_y;
	std::shared_ptr<clan::TextFieldView> input_z;

	clan::Signal<void()> &sig_value_changed() { return _sig_value_changed; }

private:
	clan::Signal<void()> _sig_value_changed;
};
