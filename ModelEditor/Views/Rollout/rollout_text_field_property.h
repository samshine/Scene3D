
#pragma once

class RolloutView;

class RolloutTextFieldProperty : public clan::View
{
public:
	RolloutTextFieldProperty(const std::string &label);

	std::shared_ptr<clan::LabelView> label;
	std::shared_ptr<clan::TextFieldView> text_field;

	clan::Signal<void()> &sig_value_changed() { return _sig_value_changed; }

private:
	clan::Signal<void()> _sig_value_changed;
};
