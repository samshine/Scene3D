
#pragma once

#include <UICore/View/view.h>
#include <UICore/StandardViews/label_view.h>
#include <UICore/StandardViews/text_field_view.h>

class RolloutView;

class RolloutTextFieldProperty : public uicore::View
{
public:
	RolloutTextFieldProperty(const std::string &label);

	std::shared_ptr<uicore::LabelView> label;
	std::shared_ptr<uicore::TextFieldView> text_field;

	uicore::Signal<void()> &sig_value_changed() { return _sig_value_changed; }

private:
	uicore::Signal<void()> _sig_value_changed;
};
